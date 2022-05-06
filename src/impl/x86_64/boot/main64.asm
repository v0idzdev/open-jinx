; -------------------------------------------- ;
; This program assists main.asm with entering  ;
; long mode                                    ;
; -------------------------------------------- ;
GLOBAL            LONG_MODE_START
SECTION          .TEXT
BITS 64

LONG_MODE_START:
    MOV           AX, 0
    MOV           SS, AX
    MOV           DS, AX
    MOV           ES, AX
    MOV           FS, AX
    MOV           GS, AX

    ; Print OK
    MOV           DWORD [0xB8000], 0x2F4B22F4F
    HLT