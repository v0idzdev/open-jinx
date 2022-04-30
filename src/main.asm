org 0x7c00  ; expect to load at address 0x7c00
bits 16     ; emit 16-bit code


main:
    hlt ; halt the processor

.halt:
    jmp .halt


times 510-($-$$) db 0 ; pad bytes for the length of the program to 0
dw 0aa55h             ; befine 2-byte constant