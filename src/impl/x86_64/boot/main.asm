; -------------------------------------------- ;
; This program is the entry point into Jinx OS ;
; -------------------------------------------- ;
GLOBAL   START
SECTION .TEXT
BITS     32

START:
    ; Print OK
    MOV DWORD [0xB8000], 0x2F4B2F4F   
    HLT