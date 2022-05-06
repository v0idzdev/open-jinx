; -------------------------------------------- ;
; This program assists main.asm with entering  ;
; long mode                                    ;
; -------------------------------------------- ;
GLOBAL            LONG_MODE_START
EXTERN            kernel_main
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
    MOV           CALL kernel_main    ; main.c
    HLT