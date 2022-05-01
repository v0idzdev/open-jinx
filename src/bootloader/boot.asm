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