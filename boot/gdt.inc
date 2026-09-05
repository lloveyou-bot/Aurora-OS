; boot/gdt.inc
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

CODE_SEG equ gdt_code - gdt_null
DATA_SEG equ gdt_data - gdt_null