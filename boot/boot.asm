; ============================================
; Aurora OS - Boot Sector
; 功能：读取第2扇区到 0x8200 并跳转
; ============================================

[ORG 0x7C00]
[BITS 16]

start:
    ; 初始化段寄存器和栈
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; 清屏
    mov ah, 0x06
    mov al, 0x00
    mov bh, 0x07
    mov cx, 0x0000
    mov dx, 0x184F
    int 0x10

    ; 显示 "Loading Stage 2..."
    mov si, msg_load
    call print

    ; ----- 复位磁盘系统 -----
    mov ah, 0x00
    mov dl, 0x00
    int 0x13
    jc disk_error

    ; ----- 读取扇区到 0x8200 -----
    mov ax, 0x0000
    mov es, ax
    mov bx, 0x8200

    mov ah, 0x02
    mov al, 0x01
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, 0x00
    int 0x13
    
    jnc disk_success

disk_error:
    mov si, msg_error
    call print
    mov al, ah
    call print_hex
    jmp $

disk_success:
    mov si, msg_ok
    call print

    ; ----- 跳转到加载的代码 -----
    jmp 0x0000:0x8200

; ---------- 打印字符串 ----------
print:
    mov ah, 0x0E
.loop:
    lodsb
    test al, al
    jz .done
    int 0x10
    jmp .loop
.done:
    ret

; ---------- 打印十六进制数 (AL) ----------
print_hex:
    push ax
    mov al, ah
    shr al, 4
    call print_nibble
    pop ax
    and al, 0x0F
    call print_nibble
    ret

print_nibble:
    add al, 0x30
    cmp al, 0x39
    jle .print
    add al, 0x07
.print:
    mov ah, 0x0E
    int 0x10
    ret

; ---------- 数据 ----------
msg_load   db "Loading Stage 2...", 0x0D, 0x0A, 0
msg_error  db "Error: 0x", 0
msg_ok     db "OK! Jumping...", 0x0D, 0x0A, 0

; ---------- 引导签名 ----------
times 510 - ($ - $$) db 0
dw 0xAA55