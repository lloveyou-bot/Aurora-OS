// kernel/src/pic.c

#define PIC1_CMD  0x20
#define PIC1_DATA 0x21
#define PIC2_CMD  0xA0
#define PIC2_DATA 0xA1

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

void pic_remap() {
    outb(PIC1_CMD, 0x11);
    outb(PIC2_CMD, 0x11);
    
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    
    // 屏蔽所有中断，只靠 pic_unmask 开启需要的
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_unmask(unsigned char irq) {
    unsigned short port;
    unsigned char value;
    
    if (irq < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}