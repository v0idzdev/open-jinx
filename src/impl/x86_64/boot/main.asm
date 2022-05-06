; -------------------------------------------- ;
; This program is the entry point into Jinx OS ;
; -------------------------------------------- ;
GLOBAL     START
SECTION   .TEXT
BITS       32

START:
    MOV    ESP, STACK_TOP                 ; Stack pointer
    CALL   CHECK_MULTIBOOT
    CALL   CHECK_CPUID
    CALL   CHECK_LONG_MODE

    ; Use paging to implement virtual memory
    ; This allows us to enter long mode
    CALL   SETUP_PAGE_TABLES
    CALL   ENABLE_PAGING

    ; Print OK
    MOV    DWORD [0xB8000], 0x2F4B2F4F
    HLT

; Stack
SECTION .BSS

STACK_BOTTOM:
    RESB   4096 * 4                       ; Reserve 16 KB of memory
STACK_TOP:

; Subroutines
; Checks for multiboot
CHECK_MULTIBOOT:
    CMP    EAX, 0x36D76289
    JNE   .NO_MULTIBOOT
    RET
.NO_MULTIBOOT:
    MOV    AL, "M"                        ; Error code M - no multiboot
    JMP    ERROR

; Displays an error code
ERROR:
    MOV    DWORD [0xB8000], 0x4F524F45
    MOV    DWORD [0xB8004], 0x4F3A4F52
    MOV    DWORD [0xB8008], 0x4F204F20
    MOV    BYTE  [0xB800A], AL
    HLT

; Check if the CPU supports CPUID
CHECK_CPUID:
    PUSHFD 
    POP    EAX
    MOV    ECX, EAX
    XOR    EAX, 1 << 21
    PUSH   EAX
    POPFD
    PUSHFD
    POP    EAX
    PUSH   ECX
    POPFD
    CMP    EAX, ECX
    JE    .NO_CPUID
    RET
.NO_CPUID:
    MOV    AL, "C"                       ; Error code C - no CPUID
    JMP    ERROR

; Check for long mode support
CHECK_LONG_MODE:
    MOV    EAX, 0x80000000
    CPUID  
    CMP    EAX, 0x80000001
    JB    .NO_LONG_MODE
    MOV    EAX, 0x80000001
    CPUID
    TEST   EDX, 1 << 29
    JZ    .NO_LONG_MODE
    RET
.NO_LONG_MODE:
    MOV    AL, "L"                      ; Error code L - no long mode 
    JMP    ERROR