; -------------------------------------------- ;
; This program is the entry point into Jinx OS ;
; -------------------------------------------- ;
GLOBAL         START
EXTERN         LONG_MODE_START
SECTION       .TEXT
BITS           32

START:
    MOV        ESP, STACK_TOP                                    ; Stack pointer
    CALL       CHECK_MULTIBOOT
    CALL       CHECK_CPUID
    CALL       CHECK_LONG_MODE

    ; Use paging to implement virtual memory
    ; This allows us to enter long mode
    CALL       SETUP_PAGE_TABLES
    CALL       ENABLE_PAGING

    LGDT       [GDT64.POINTER]
    JMP        GDT64.CODE_SEGMENT:LONG_MODE_START
    HLT

; Subroutines
; Checks for multiboot
CHECK_MULTIBOOT:
    CMP        EAX, 0x36D76289
    JNE       .NO_MULTIBOOT
    RET
.NO_MULTIBOOT:
    MOV        AL, "M"                                           ; Error code M - no multiboot
    JMP        ERROR

; Check if the CPU supports CPUID
CHECK_CPUID:
    PUSHFD 
    POP        EAX
    MOV        ECX, EAX
    XOR        EAX, 1 << 21
    PUSH       EAX
    POPFD
    PUSHFD
    POP        EAX
    PUSH       ECX
    POPFD
    CMP        EAX, ECX
    JE        .NO_CPUID
    RET
.NO_CPUID:
    MOV        AL, "C"                                           ; Error code C - no CPUID
    JMP        ERROR

; Check for long mode support
CHECK_LONG_MODE:
    MOV        EAX, 0x80000000
    CPUID  
    CMP        EAX, 0x80000001
    JB        .NO_LONG_MODE
    MOV        EAX, 0x80000001
    CPUID
    TEST       EDX, 1 << 29
    JZ        .NO_LONG_MODE
    RET
.NO_LONG_MODE:
    MOV        AL, "L"                                           ; Error code L - no long mode 
    JMP        ERROR 

; Sets up page tables
SETUP_PAGE_TABLES:
    MOV        EAX, PAGE_TABLE_L3
    OR         EAX, 0B11                                         ; Present/writable
    MOV        [PAGE_TABLE_L4], EAX
    MOV        EAX, PAGE_TABLE_L2
    OR         EAX, 0B11
    MOV        [PAGE_TABLE_L3], EAX
    MOV        ECX, 0                                            ; Counter
.LOOP:                                                           ; For loop
    MOV        EAX, 0x200000                                     ; 2MiB
    MUL        ECX
    OR         EAX, 0b10000011
    MOV        [PAGE_TABLE_L2 + ECX * 8], EAX
    INC        ECX                                               ; Increment counter
    CMP        ECX, 512
    JNE       .LOOP
    RET

; Implements virtual memory using paging
ENABLE_PAGING:
    MOV        EAX, PAGE_TABLE_L4
    MOV        CR3, EAX

    ; Enable PAE
    MOV        EAX, CR4
    OR         EAX, 1 << 5
    MOV        CR4, EAX

    ; Enable long mode
    MOV        ECX, 0xC0000080
    RDMSR
    OR         EAX, 1 << 8
    WRMSR

    ; Enable paging
    MOV        EAX, CR0
    OR         EAX, 1 << 31
    MOV        CR0, EAX
    RET

; Displays an error code
ERROR:
    MOV        DWORD [0xB8000], 0x4F524F45
    MOV        DWORD [0xB8004], 0x4F3A4F52
    MOV        DWORD [0xB8008], 0x4F204F20
    MOV        BYTE  [0xB800A], AL
    HLT

; Stack
SECTION       .BSS
ALIGN          4096

; Page tables
PAGE_TABLE_L4:
    RESB       4096
PAGE_TABLE_L3:
    RESB       4096
PAGE_TABLE_L2:
    RESB       4096                                             ; L1 page table not needed

STACK_BOTTOM:
    RESB       4096 * 4                                         ; Reserve 16 KB of memory
STACK_TOP:

; Read only data
; Global descriptor table
SECTION       .RODATA
GDT64:
    DQ     0 
.CODE_SEGMENT: EQU $ - GDT64
    DQ         (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)    ; Code segment
.POINTER:
    DW         $ - GDT64 - 1
    DQ         GDT64