; --------------------------------------------- ;
; Includes data compliant with the multiboot2   ;
; specification, which most bootloaders support ;
; --------------------------------------------- ;
SECTION .MULTIBOOT_HEADER

HEADER_START:
    DD 0xE85250D6                                                     ; Magic number
    DD 0                                                              ; Protected mode i386
    DD HEADER_END - HEADER_START                                      ; Header length
    DD 0x100000000 - (0xE85250D6 + 0 + (HEADER_END - HEADER_START))    ; Checksum
    
    ; End tag
    DW 0
    DW 0
    DD 8
HEADER_END: