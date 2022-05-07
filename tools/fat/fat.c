#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * Struct: bool
 * ------------
 *
 * This structure implements a boolean data type by redefining 1 to
 * be true and 0 to be false
 */
typedef uint8_t bool;
#define true 1
#define false 0

/*
 * Struct: BootSector
 * ------------------
 *
 * This structure will contain data included in the FAT12 header
 */
typedef struct {
    uint8_t bootJumpInstruction[3];
    uint8_t oemIdentifier[8];
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t fatCount;
    uint16_t dirEntryCount;
    uint16_t totalSectors;
    uint8_t mediaDescriptorType;
    uint16_t sectorsPerFat;
    uint16_t sectorsPerTrack;
    uint16_t heads;
    uint32_t hiddenSectors;
    uint32_t largeSectorCount;

    // The members below refer to the Extended Boot Record
    uint8_t driveNumber;
    uint8_t _reserved;
    uint8_t signature;
    uint32_t volumeID;        // This is the disk's serial number
    uint8_t volumeLabel[11];  // This uses 11 bytes, padded with spaces
    uint8_t systemID[8];

    // ... We don't care about code ...

} __attribute__((packed)) BootSector;

/*
 * Struct: DirectoryEntry
 * ----------------------
 *
 * This structure contains all of the fields for a directory entry that are
 * present in the FAT specifiecation
 */
typedef struct {
    uint8_t name[11];
    uint8_t attributes;
    uint8_t _reserved;
    uint8_t createdTimeTenths;
    uint16_t creationTime;
    uint16_t creationDate;
    uint16_t accessedDate;
    uint16_t firstClusterHigh;
    uint16_t modifiedTime;
    uint16_t modifiedDate;
    uint16_t firstClusterLow;
    uint32_t size;

} __attribute__((packed)) DirectoryEntry;

// Function declarations
bool ReadBootSector(FILE* disk);
bool ReadDiskSectors(FILE* disk, uint8_t lba, uint32_t count, void* bufferOut);
bool ReadFAT(FILE* disk);
bool ReadRootDirectory(FILE* disk);
DirectoryEntry* FindFile(const char* name);
bool ReadFile(DirectoryEntry* fileEntry, FILE* disk, uint8_t* outputBuffer);

// Global variables
BootSector g_BootSector;  // This will contain a FAT12 header and EBR
uint8_t* g_Fat = NULL;    // This will contain the disk's allocation table
DirectoryEntry* g_RootDirectory = NULL;
uint32_t g_RootDirectoryEnd;

/*
 * Function: main
 * --------------
 *
 * \brief This function is where the program's execution starts and finishes
 *
 * @param argc This refers to the number of command line arguments the
 *             program was run with
 * @param argv This contains the command line arguments that the program
 *             was run with
 */
int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Syntax: %d <disk image> <file name>\n", argv[0]);
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb");
    if (!disk) {
        fprintf(stderr, "Cannot open disk image %s\n", argv[1]);
        return -1;
    }

    // ReadBootSector will return a boolean value indicating the success
    // of the operation
    if (!ReadBootSector(disk)) {
        fprintf(stderr, "Could not read boot sector\n");
        return -2;
    }

    // The same behaviour as descibed above also applies to the conditional
    // statements written below
    if (!ReadFAT(disk)) {
        fprintf(stderr, "Could not read file allocation table.\n");
        free(g_Fat);
        return -3;
    }

    if (!ReadRootDirectory(disk)) {
        fprintf(stderr, "Could not read file allocation table.\n");
        free(g_Fat);
        free(g_RootDirectory);
        return -4;
    }

    DirectoryEntry* fileEntry = FindFile(argv[2]);
    if (!fileEntry) {
        fprintf(stderr, "Could not find file %s.\n", argv[2]);
        free(g_Fat);
        free(g_RootDirectory);
        return -5;
    }

    uint8_t* buffer = (uint8_t*)malloc(fileEntry->size + g_BootSector.bytesPerSector);
    if (!ReadFile(fileEntry, disk, buffer)) {
        fprintf(stderr, "Could not read file %s.\n", argv[2]);
        free(g_Fat);
        free(g_RootDirectory);
        free(buffer);
        return -5;
    }

    // The following code loops over each character in the file
    // If it is printable, output it. Otherwise, output it's hexadecimal value
    for (size_t i = 0; i < fileEntry->size; i++) {
        if (isprint(buffer[i])) fputc(buffer[i], stdout);
        else printf("<%02x>", buffer[i]);
    }

    printf("\n");
    free(g_Fat);
    free(g_RootDirectory);
    return 0;
}

/*
 * Function: ReadBootSector
 * ------------------------
 *
 * \brief This function reads the boot sector from the disk and stores it
 * in a global variable
 *
 * @param disk This should be the disk that the boot sector will be read from
 * @returns    This function returns a boolean value indicating the operation's
 *             success
 */
bool ReadBootSector(FILE* disk) {
    return fread(&g_BootSector, sizeof(g_BootSector), 1, disk) > 0;
}

/*
 * Function: ReadDiskSectors
 * -------------------------
 *
 * \brief This function reads a specified number of sectors from a disk and writes
 * it to a specified location
 *
 * @param disk      This should be the disk that the boot sector will be read from
 * @param lba       This should be the sector number, using Logical Block Addressing
 * @param count     This should be the number of sectors to read
 * @param bufferOut This should be a pointer to the memory location in which to
 *                  store the data
 * @returns         This function returns a boolean value indicating the operation's
 *                  success
 */
bool ReadDiskSectors(FILE* disk, uint8_t lba, uint32_t count, void* bufferOut) {
    bool ok = true;

    // The code below will attempt to read the sectors of the disk given the
    // specified parameters
    // 
    // The boolean value `ok` indicates the operation's success
    ok = ok && (fseek(disk, lba * g_BootSector.bytesPerSector, SEEK_SET) == 0);
    ok = ok && (fread(bufferOut, g_BootSector.bytesPerSector, count, disk) == count);

    return ok;
}

/*
 * Function: ReadFAT
 * -----------------
 *
 * \brief This function attempts to read a disk's FAT allocation table into
 * memory
 *
 * @param disk This should be the disk from which the allocation table will
 *             be read
 * @returns    This function returns a boolean value indicating the operation's
 *             success
 */
bool ReadFAT(FILE* disk) {
    g_Fat = (uint8_t*)
        malloc(g_BootSector.sectorsPerFat * g_BootSector.bytesPerSector);

    // After allocating a sufficient amount of memory, ReadDiskSectors is called
    // to read the disk's file allocation table
    return ReadDiskSectors(disk, g_BootSector.reservedSectors,
        g_BootSector.sectorsPerFat, g_Fat);
}

/*
 * Function: ReadRootDirectory
 * ---------------------------
 *
 * \brief This function attempts to read the root directory of the disk's file
 * system
 *
 * @param disk This should be the disk from which the allocation table will
 *             be read
 * @returns    This function returns a boolean value indicating the operation's
 *             success
 */
bool ReadRootDirectory(FILE* disk) {
    // LBA is the sum of the sizes of the previous two regions
    //
    // LBA refers the the logical block address
    uint32_t lba = g_BootSector.reservedSectors + g_BootSector.sectorsPerFat
        * g_BootSector.fatCount;

    uint32_t size = sizeof(DirectoryEntry) * g_BootSector.dirEntryCount;
    uint32_t sectors = (size / g_BootSector.bytesPerSector);

    if (size % g_BootSector.bytesPerSector > 0)
        sectors++;

    // Allocate sufficient memory for the file system's root directory
    g_RootDirectoryEnd = lba + sectors;
    g_RootDirectory = (DirectoryEntry*)malloc(sectors * g_BootSector.bytesPerSector);
    return ReadDiskSectors(disk, lba, sectors, g_RootDirectory);
}

/*
 * Function: FindFile
 * ------------------
 *
 * \brief This function attempts to find a specified file
 *
 * @param name This should be a string corresponding to the name of the file
 * @returns    This function returns a pointer to the DirectoryEntry to read
 *             from the specified disk
 */
DirectoryEntry* FindFile(const char* name) {
    for (uint32_t i = 0; i < g_BootSector.dirEntryCount; i++) {

        // If the name parameter is the same as the name passed in, return
        // the memory address of the directory entry
        if (memcmp(name, g_RootDirectory[i].name, 11) == 0)
            return &g_RootDirectory[i];
    }

    return NULL;
}

/*
 * Function: ReadFile
 * ------------------
 *
 * \brief This function attempts to read a DirectoryEntry and output it to
 * a buffer
 *
 * @param fileEntry    This should be the DirectoryEntry to read
 * @param disk         This should be the disk to read the file from
 * @param outputBuffer This should be the buffer to write output to
 * @returns            This function returns a boolean value indicating the
 *                     operation's success
 */
bool ReadFile(DirectoryEntry* fileEntry, FILE* disk, uint8_t* outputBuffer) {
    uint16_t currentCluster = fileEntry->firstClusterLow;
    bool ok = true;

    do {
        uint32_t lba = g_RootDirectoryEnd + (currentCluster - 2) * g_BootSector.sectorsPerCluster;
        ok = ok && ReadDiskSectors(disk, lba, g_BootSector.sectorsPerCluster, outputBuffer);
        outputBuffer += g_BootSector.sectorsPerCluster * g_BootSector.bytesPerSector;

        uint32_t fatIndex = currentCluster * 3 / 2;

        if (currentCluster % 2 == 0)
            currentCluster = (*(uint16_t*)(g_Fat + fatIndex)) && 0x0FFF;
        else
            currentCluster = (*(uint16_t*)(g_Fat + fatIndex)) >> 4;

    } while (ok && currentCluster < 0x0FF8);

    return ok;
}