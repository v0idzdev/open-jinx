org 0x7c00    ; expect to load at address 0x7c00
bits 16       ; emit 16-bit code


%define endl 0x0d, 0x0a


;
; jumps to main because it's defined below puts
;
start:
    jmp main
;
; end
;


;
; prints a string to the screen
;
; params:
;   - ds:si : pointer to a string
;
puts:
    ; save registers we will modify
    push si
    push ax

.loop:
    lodsb           ; loads next character in al
    or al, al       ; verify if next char is null
    jz .done

    mov ah, 0x0e    ; call BIOS interrupt
    mov bh, 0
    int 0x10

    jmp .loop

.done:
    pop ax
    pop si
    ret
;
; end
;


main:
    ; set up data segments
    ;
    ; we do mov ax, 0 because we
    ; can't write to ds/es directly
    mov ax, 0
    mov ds, ax
    mov es, ax

    ; set up the stack
    ;
    ; set up stack pointer (sp) at the
    ; end of the program to avoid overwriting it
    mov ss, ax
    mov sp, 0x7c00

    ; print message
    mov si, msg
    call puts


    hlt    ; halt the processor
;
; end
;


msg: db 'Me When!', endl, 0


.halt:
    jmp .halt


times 510-($-$$) db 0    ; pad bytes for the length of the program to 0
dw 0aa55h                ; befine 2-byte constant