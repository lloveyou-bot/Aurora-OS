#include "terminal.h"
#include "shell.h"
#include "fs.h"
#include "keyboard.h"
#include "idt.h"

void pic_remap(void);
void pic_unmask(unsigned char irq);
void pic_mask(unsigned char irq);

void kmain() {
    terminal_clear();
    terminal_write("Aurora OS v0.5 (Text Mode + Network)\n");
    terminal_write("Initializing...\n");

    idt_init();
    pic_remap();
    keyboard_init();
    fs_init();

    terminal_write("Ready.\n");
    terminal_write("Type /help for commands.\n");
    terminal_write("Type /net to test network.\n");

    // ====== 关键！初始化 Shell，显示提示符 ======
    shell_init();
    
    __asm__ volatile ("sti");
    shell_run();
}