; boot/interrupt.asm
[BITS 32]

global load_idt
global keyboard_handler_int

extern keyboard_handler

; 加载 IDTR
load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    ret

; 键盘中断入口
keyboard_handler_int:
    pushad
    call keyboard_handler
    popad
    iretd