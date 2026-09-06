; ============================================
; Aurora OS - Installer Loader
; ============================================

[ORG 0x1000]
[BITS 16]

start:
    mov si, msg_loading
    call print_16

    ; 读取 installer 到 0x2000
    mov ax, 0x2000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 0x20
    mov ch, 0x00
    mov cl, 0x04
    mov dh, 0x00
    mov dl, 0x00
    int 0x13
    jc .error

    mov si, msg_ok
    call print_16

    ; 显示 'J' 表示即将跳转
    mov si, msg_jump
    call print_16

    ; 跳转到 installer
    jmp 0x2000:0x0000

.error:
    mov si, msg_error
    call print_16
    jmp $

print_16:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

msg_loading db "Loading installer...", 0x0D, 0x0A, 0
msg_ok      db "OK", 0x0D, 0x0A, 0
msg_jump    db "J", 0x0D, 0x0A, 0
msg_error   db "Error!", 0x0D, 0x0A, 0

times 512 - ($ - $$) db 0