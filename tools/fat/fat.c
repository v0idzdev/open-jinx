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
} BootSector;

/*
 * Function: ReadBootSector
 * ------------------------
 *
 * This function reads the boot sector from the disk and stores it
 * in a global variable
 *
 * disk: This should be the disk that the boot sector will be read from
 *
 * returns: This function returns a boolean value indicating the operation's
 *          success
 */
bool ReadBootSector(FILE* disk) {
}

/*
 * Function: main
 * --------------
 *
 * This function is where the program's execution starts and finishes
 *
 * argc: This number refers to the number of command line arguments the
 *       program was run with
 *
 * argv: This vector contains the command line arguments that the program
 *       was run with
 */
int main(int argc, char** argv) {
    if (argc < 3) {
        printf("Syntax: %d <disk image> <file name>\n", argv[0]);
        return -1;
    }

    FILE* disk = fopen(argv[1], "rb");

    return 0;
}