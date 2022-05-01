org 0x7c00    ; expect to load at address 0x7c00
bits 16       ; emit 16-bit code

%define endl 0x0d, 0x0a


;
; FAT12 header
;
jmp short start
nop

bdb_oem:                      db 'MSWIN4.1'            ; 8 bytes
bdb_bytes_per_sector:         dw 512
bdb_sectors_per_cluster:      db 1
bdb_reserved_sectors:         dw 1
bdb_fat_count:                db 2
bdb_dir_entries_count:        dw 0e0h
bdb_total_sectors:            dw 2880                  ; 1.44 mb
bdb_media_descriptor_type:    db 0f0h                  ; 3.5" floppy disk
bdb_sectors_per_fat:          dw 9
bdb_sectors_per_track:        dw 18
bdb_heads:                    dw 2
bdb_hidden_sectors:           dd 0
bdb_large_sector_count:       dd 0
;
; end
;


;
; extended boot record
;
ebr_drive_number:             db 0                     ; 0x00 floppy, 0x80 hdd
                              db 0                     ; reserved byte
ebr_signature:                db 29h
ebr_volume_id:                db 12h, 34h, 56h, 78h    ; serial number
ebr_volume_label:             db 'Jinx OS    '         ; 11 bytes
ebr_system_id:                db 'FAT12   '            ; 8 bytes
;
; end
;


;
; main code
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

    ; [TEST] read something from floppy disk
    ; bios should set dl to drive number
    mov [ebr_drive_number], dl

    mov ax, 1         ; lba=1, second sector from disk
    mov cl, 1         ; 1 sector to read
    mov bx, 0x7e00    ; data should be after the bootloader
    call disk_read

    ; print message
    mov si, msg_success
    call puts

    cli               ; disable interrupts
    hlt               ; halt the processor
;
; end
;


;
; error handlers
;

;
; shows error message on disk read failure
;
floppy_error:
    mov si, msg_read_failed
    call puts
    jmp wait_key_and_reboot
;
; end
;


;
; waits for key press and reboots
;
wait_key_and_reboot:
    mov ah, 0
    int 16h         ; wait for key press
    jmp 0ffffh:0    ; jump to beginning of BIOS, should reboot

.halt:
    cli             ; disable interrupts
    hlt
;
; end
;


;
; disk routines
;

;
; converts an lba address to a chs address
;
; params:
;   - ax : lba address
;
; returns:
;   - cx (bits 0-5) : sector number
;   - cx (buts 6-15) : cyclinder
;   - dh : head
;
lba_to_chs:
    ; push ax, dx to the stack
    push ax
    push dx

    xor dx, dx                          ; dx = 0
    div word [bdb_sectors_per_track]    ; ax = lba / sectors per track
                                        ; dx = lba % sectors per track
    inc dx                              ; dx = (lba / sectors per track) + 1 = sector
    mov cx, dx                          ; cx = sector
    
    xor dx, dx                          ; dx = 0
    div word [bdb_heads]                ; ax = (lba / sectors per track) / heads = cylinder
                                        ; ax = (lba / sectors per track) % heads = head
    mov dh, dl                          ; dl = head
    mov ch, al                          ; ch = cylinder (lower 8 bits)
    shl ah, 6
    or cl, ah                           ; upper 2 bits of cylinder in cl         

    pop ax
    mov dl, al                          ; restore dl
    pop ax
    ret
;
; end
;


;
; reads sectors from a disk
;
; parameters:
;   - ax : lba address
;   - cl : number of sectors to read (up to 128)
;   - dl : drive number
;   - es/bx : memory address to store read data
;
disk_read:
    ; save registers we will modify
    push ax
    push bx
    push cx
    push dx
    push di

    push cx            ; temporarily save cl
    call lba_to_chs    ; compute chs
    pop ax             ; al = num sectors to read

    mov ah, 02h
    mov di, 3          ; retry count

.retry:
    pusha              ; save all registers, we don't know what bios modifies
    stc                ; set carry flag

    int 13h            ; carry flag cleared - success
    jnc .done          ; jump if no carry set
    
    ; read failed
    popa
    call disk_reset

    dec di
    test di, di
    jnz .retry

.fail:
    ; after all attempts are exhausted
    jmp floppy_error

.done:
    popa

    pop di            ; restore registers modified
    pop dx
    pop cx
    pop bx
    pop ax
    ret
;
; end
;


;
; resets disk controller
;
; params:
;   - dl: drive number
;
disk_reset:
    pusha
    mov ah, 0
    stc
    int 13h
    jc floppy_error
    popa
    ret
;
; end
;


msg_success:     db 'Successfully booted.', endl, 0
msg_read_failed: db 'Failed to read from disk.', endl, 0

times 510-($-$$) db 0    ; pad bytes for the length of the program to 0
dw 0aa55h                ; befine 2-byte constant