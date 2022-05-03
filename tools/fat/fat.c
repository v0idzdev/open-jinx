#include <stdint.h>
#include <stdio.h>

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

bool ReadBootSector(FILE* disk);
bool ReadDiskSectors(FILE* disk, uint8_t lba, uint32_t count, void* bufferOut);
bool ReadAllocationTable(FILE* disk);

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
struct {
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

BootSector g_BootSector;  // This will contain a FAT12 header and EBR
uint8_t g_Fat = NULL;     // This will contain the disk's allocation table

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

    if (!ReadAllocationTable(disk)) {
        fprintf(stderr, "Could not read file allocation table.\n");
        free(g_Fat);
        return -3;
    }

    free(g_Fat);
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
 * Function: ReadAllocationTable
 * -----------------------------
 *
 * \brief This function attempts to read a disk's FAT allocation table into
 * memory
 *
 * @param disk This should be the disk from which the allocation table will
 *             be read
 * @returns    This function returns a boolean value indicating the operation's
 *             success
 */
bool ReadAllocationTable(FILE* disk) {
    g_Fat = (uint8_t*)
        malloc(g_BootSector.sectorsPerFat * g_BootSector.bytesPerSector);

    // After allocating a sufficient amount of memory, ReadDiskSectors is called
    // to read the disk's file allocation table
    return ReadDiskSectors(disk, g_BootSector.reservedSectors,
        g_BootSector.sectorsPerFat, g_Fat);
}