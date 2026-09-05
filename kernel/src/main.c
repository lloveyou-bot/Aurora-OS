// kernel/src/main.c
#include <stddef.h>
#include "terminal.h"
#include "shell.h"
#include "fs.h"
#include "graphics/graphics.h"
#include "graphics/vbe.h"
#include "font/hzk16.h"

extern void idt_init();
extern void load_idt_entry(unsigned char num, unsigned int base, unsigned short selector, unsigned char flags);
extern void pic_remap();
extern void pic_unmask(unsigned char irq);
extern void keyboard_init();
extern void keyboard_handler();
extern void keyboard_handler_int();

void kmain() {
    terminal_clear();
    terminal_write("Aurora OS v0.4\n");
    terminal_write("Initializing...\n");
    
    idt_init();
    pic_remap();
    load_idt_entry(0x21, (unsigned int)keyboard_handler_int, 0x08, 0x8E);
    keyboard_init();
    pic_unmask(1);
    
    fs_init();
    
    // 图形模式暂时禁用
    // graphics_init();
    terminal_write("Text mode only.\n");
    
    shell_init();
    __asm__ volatile ("sti");
    shell_run();
}