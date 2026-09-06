// kernel/src/idt.c
#include <stddef.h>

#define IDT_SIZE 256

struct idt_entry {
    unsigned short offset_low;
    unsigned short selector;
    unsigned char zero;
    unsigned char flags;
    unsigned short offset_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

static struct idt_entry idt[IDT_SIZE];
static struct idt_ptr idtp;

extern void load_idt(unsigned int);

void load_idt_entry(unsigned char num, unsigned int base, unsigned short selector, unsigned char flags) {
    if (num >= IDT_SIZE) return;
    idt[num].offset_low = base & 0xFFFF;
    idt[num].offset_high = (base >> 16) & 0xFFFF;
    idt[num].selector = selector;
    idt[num].flags = flags;
    idt[num].zero = 0;
}

void idt_init() {
    idtp.limit = sizeof(struct idt_entry) * IDT_SIZE - 1;
    idtp.base = (unsigned int)&idt;
    
    for (int i = 0; i < IDT_SIZE; i++) {
        load_idt_entry(i, 0, 0, 0);
    }
    
    load_idt((unsigned int)&idtp);
}