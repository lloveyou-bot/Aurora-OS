// kernel/src/mouse.c
#include "ports.h"
#include "terminal.h"

static unsigned char packet[3];
static int packet_index = 0;
static int mouse_x = 40;
static int mouse_y = 12;

void mouse_init() {
    outb(0x64, 0xA8);
    outb(0x64, 0x20);
    unsigned char config = inb(0x60);
    config |= 0x02;
    outb(0x64, 0x60);
    outb(0x60, config);
    outb(0x64, 0xD4);
    outb(0x60, 0xF4);
    packet_index = 0;
}

void mouse_handler() {
    unsigned char data = inb(0x60);
    packet[packet_index++] = data;
    
    if (packet_index >= 3) {
        packet_index = 0;
        if (!(packet[0] & 0x08)) return;
        
        int dx = (int)packet[1];
        int dy = -(int)packet[2];
        
        mouse_x += dx / 2;
        mouse_y += dy / 2;
        
        if (mouse_x < 0) mouse_x = 0;
        if (mouse_x > 79) mouse_x = 79;
        if (mouse_y < 0) mouse_y = 0;
        if (mouse_y > 24) mouse_y = 24;
        
        terminal_write_at("Mouse: ", 24, 50, 0x0F);
        terminal_putchar_at('0' + (mouse_x / 10), 24, 58, 0x0F);
        terminal_putchar_at('0' + (mouse_x % 10), 24, 59, 0x0F);
        terminal_write_at(",", 24, 60, 0x0F);
        terminal_putchar_at('0' + (mouse_y / 10), 24, 62, 0x0F);
        terminal_putchar_at('0' + (mouse_y % 10), 24, 63, 0x0F);
    }
    
    outb(0x20, 0x20);
    outb(0xA0, 0x20);
}