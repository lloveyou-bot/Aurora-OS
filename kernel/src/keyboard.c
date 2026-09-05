// kernel/src/keyboard.c - 支持 Shell 和 Explorer 模式切换
#include "terminal.h"
#include "shell.h"
#include "explorer.h"

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

static unsigned char shift_state = 0;
#define SHIFT_LEFT  0x01
#define SHIFT_RIGHT 0x02
#define CAPS_LOCK   0x04

static int explorer_active = 0;

static char scancode_to_ascii(unsigned char scancode) {
    if (scancode & 0x80) return 0;
    
    static const char map_normal[] = {
        0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',  0,   0,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',  0,   0,   'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,  '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', ',', '.', '/',  0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
    };
    
    static const char map_shift[] = {
        0,   0,   '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',  0,   0,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',  0,   0,   'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,  '|', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<', '>', '?',  0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1',
        '2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0
    };
    
    const char* map = (shift_state & (SHIFT_LEFT | SHIFT_RIGHT)) ? map_shift : map_normal;
    
    if (scancode >= sizeof(map_normal)) return 0;
    char c = map[scancode];
    
    if (shift_state & CAPS_LOCK) {
        if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
        else if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
    }
    
    return c;
}

static void update_modifier(unsigned char scancode, unsigned char is_press) {
    if (scancode == 0x2A) {
        if (is_press) shift_state |= SHIFT_LEFT;
        else shift_state &= ~SHIFT_LEFT;
    }
    if (scancode == 0x36) {
        if (is_press) shift_state |= SHIFT_RIGHT;
        else shift_state &= ~SHIFT_RIGHT;
    }
    if (scancode == 0x3A && is_press) {
        shift_state ^= CAPS_LOCK;
    }
}

void keyboard_init() {
    shift_state = 0;
    explorer_active = 0;
}

void keyboard_set_explorer_mode(int active) {
    explorer_active = active;
}

void keyboard_handler() {
    unsigned char scancode = inb(0x60);
    unsigned char is_press = !(scancode & 0x80);
    unsigned char raw = scancode & 0x7F;
    
    update_modifier(raw, is_press);
    
    if (is_press) {
        // Explorer 模式
        if (explorer_active) {
            if (raw == 0x01) {
                int should_exit = explorer_process_key(27);
                if (should_exit) {
                    explorer_active = 0;
                    terminal_clear();
                    shell_init();
                }
                outb(0x20, 0x20);
                return;
            }
            if (raw == 0x1C) {
                explorer_process_key('\n');
                outb(0x20, 0x20);
                return;
            }
            char ascii = scancode_to_ascii(raw);
            if (ascii != 0) {
                explorer_process_key(ascii);
            }
            outb(0x20, 0x20);
            return;
        }
        
        // Shell 模式
        if (raw == 0x0E) {
            shell_process_char('\b');
            outb(0x20, 0x20);
            return;
        }
        if (raw == 0x1C) {
            shell_process_char('\n');
            outb(0x20, 0x20);
            return;
        }
        
        char ascii = scancode_to_ascii(raw);
        if (ascii != 0) {
            shell_process_char(ascii);
        }
    }
    
    outb(0x20, 0x20);
}