#ifndef IDT_H
#define IDT_H

#define IDT_SIZE 256

struct idt_entry {
    unsigned short offset_low;   // 处理函数偏移低16位
    unsigned short selector;     // 段选择子（0x08）
    unsigned char zero;          // 保留，必须为0
    unsigned char flags;         // 属性（0x8E = 中断门，32位）
    unsigned short offset_high;  // 处理函数偏移高16位
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;        // IDT大小 - 1
    unsigned int base;           // IDT基址
} __attribute__((packed));

extern void load_idt(unsigned int idt_ptr_addr);
void idt_init();
void load_idt_entry(unsigned char num, unsigned long base, unsigned short selector, unsigned char flags);

#endif