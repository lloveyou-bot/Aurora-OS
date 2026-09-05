; ============================================
; Aurora OS - Boot Sector (硬盘版)
; ============================================

[ORG 0x7C00]
[BITS 16]

start:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov si, msg
    call print

    ; 从硬盘读取 loader（LBA 1）
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    ; 使用 LBA 读取（扩展读）
    mov ah, 0x42
    mov si, dap
    mov dl, 0x80
    int 0x13
    jc error

    jmp 0x1000:0x0000

error:
    mov si, err_msg
    call print
    ; 显示错误码
    mov al, ah
    call print_hex
    jmp $

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

print_hex:
    pusha
    mov ah, 0x0E
    mov al, '0'
    int 0x10
    mov al, 'x'
    int 0x10
    popa
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
msg     db "Loading Aurora OS from HDD...", 0x0D, 0x0A, 0
err_msg db "Error: ", 0

; 磁盘地址包 (DAP)
dap:
    db 0x10        ; 大小
    db 0x00        ; 保留
    dw 0x02        ; 读取 2 个扇区
    dw 0x1000      ; 偏移
    dw 0x0000      ; 段
    dq 0x01        ; LBA 起始（第 1 扇区）

times 510 - ($ - $$) db 0
dw 0xAA55