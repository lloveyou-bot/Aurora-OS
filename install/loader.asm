; ============================================
; Aurora OS - Installer Loader (调试版)
; ============================================

[ORG 0x1000]
[BITS 16]

start:
    ; 显示 'L' 表示 loader 启动
    mov si, msg_l
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

    ; 显示 'R' 表示读取完成
    mov si, msg_r
    call print_16

    ; 检查是否成功
    jnc .success

    ; 失败：显示 'E'
    mov si, msg_e
    call print_16
    jmp $

.success:
    ; 成功：显示 'O' 然后跳转
    mov si, msg_o
    call print_16

    jmp 0x2000:0x0000

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

msg_l db "L", 0
msg_r db "R", 0
msg_e db "E", 0
msg_o db "O", 0

times 512 - ($ - $$) db 0