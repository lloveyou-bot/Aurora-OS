; ============================================
; Aurora OS - 32-bit Protected Mode Code
; 位置：加载到 0x8200 之后，跳转到这里执行
; ============================================

[BITS 32]                    ; 告诉NASM生成32位代码

pm_start:
    ; 此时所有段寄存器已经是保护模式的段选择子
    ; DS、ES、SS 已经在 loader.asm 中设置好了

    ; 显示一个字符（直接写显存 0xB8000）
    mov byte [0xB8000], 'A'   ; 字符 'A'
    mov byte [0xB8001], 0x0F  ; 白色文字，黑色背景

    ; 再显示一个字符
    mov byte [0xB8002], 'U'
    mov byte [0xB8003], 0x0F

    mov byte [0xB8004], 'R'
    mov byte [0xB8005], 0x0F

    mov byte [0xB8006], 'O'
    mov byte [0xB8007], 0x0F

    mov byte [0xB8008], 'R'
    mov byte [0xB8009], 0x0F

    mov byte [0xB800A], 'A'
    mov byte [0xB800B], 0x0F

    ; 显示字符串也可以，但要循环写

    ; 死循环
    jmp $