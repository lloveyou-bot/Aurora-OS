; ============================================
; Aurora OS - Stage 2 Loader
; ============================================

[ORG 0x8200]
[BITS 16]

start:
    mov si, msg_loading
    call print_16

    call load_kernel

    call enable_a20

    cli
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp 0x08:pm_entry

load_kernel:
    mov si, msg_kernel
    call print_16

    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 0x20
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
    ret

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

enable_a20:
    mov ax, 0x2401
    int 0x15
    ret

msg_loading    db "Loading C kernel...", 0x0D, 0x0A, 0
msg_kernel     db "Reading kernel from disk...", 0x0D, 0x0A, 0
msg_load_ok    db "Kernel loaded, jumping to protected mode...", 0x0D, 0x0A, 0
msg_load_error db "Kernel load error!", 0x0D, 0x0A, 0

; GDT
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

[BITS 32]
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000
    mov ebp, 0x90000

    ; 调试：显示 P
    mov byte [0xB8000], 'P'
    mov byte [0xB8001], 0x0F

    ; 复制内核到 0x100000
    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, 16384
    rep movsd

    ; 调试：显示 K
    mov byte [0xB8002], 'K'
    mov byte [0xB8003], 0x0F

    ; 调试：显示 J
    mov byte [0xB8004], 'J'
    mov byte [0xB8005], 0x0F

    ; 跳转到 C 内核
    jmp 0x100000

    ; 如果失败
    mov byte [0xB8006], 'F'
    mov byte [0xB8007], 0x0F
    jmp $

times 512 - ($ - $$) db 0