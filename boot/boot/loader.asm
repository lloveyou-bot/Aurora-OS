; ============================================
; Aurora OS - Stage 2 Loader (纯文本版)
; ============================================

[ORG 0x8200]
[BITS 16]

start:
    mov si, msg_loading
    call print_16

    ; 设置文本模式
    mov ax, 0x0003
    int 0x10

    call load_kernel
    call enable_a20

    cli
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp 0x08:pm_entry

; ============================================
; 加载内核
; ============================================
load_kernel:
    mov si, msg_kernel
    call print_16

    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 0x7F
    mov ch, 0x00
    mov cl, 0x03
    mov dh, 0x00
    mov dl, 0x00
    int 0x13
    jc .error

    mov si, msg_load_ok
    call print_16
    ret

.error:
    mov si, msg_load_error
    call print_16
    jmp $
    ret

; ============================================
; 辅助函数
; ============================================
print_16:
    mov ah, 0x0E
.l:
    lodsb
    test al, al
    jz .d
    int 0x10
    jmp .l
.d:
    ret

enable_a20:
    mov ax, 0x2401
    int 0x15
    ret

; ============================================
; 消息字符串
; ============================================
msg_loading     db 0x0D, 0x0A, "Aurora OS Loader v2 (Text Mode)", 0x0D, 0x0A, 0
msg_kernel      db "Loading kernel...", 0x0D, 0x0A, 0
msg_load_ok     db "Kernel loaded!", 0x0D, 0x0A, 0
msg_load_error  db "ERROR: Kernel load failed!", 0x0D, 0x0A, 0

; ============================================
; GDT
; ============================================
gdt_null:
    dd 0x00000000
    dd 0x00000000

gdt_code:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00

gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00

gdt_end:

gdt_desc:
    dw gdt_end - gdt_null - 1
    dd gdt_null

; ============================================
; 保护模式入口
; ============================================
[BITS 32]
pm_entry:
    ; 写 'P' 到显存（调试）
    mov word [0xB8000], 0x0F50
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000
    mov ebp, 0x90000

    ; ---- 复制内核到 1MB ----
    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, 16384
    rep movsd

    ; ---- 写 'K' 到显存（调试） ----
    mov word [0xB8002], 0x0F4B

    ; ---- 跳转到 C 内核 ----
    jmp 0x100000

; ============================================
; 文件结束
; ============================================