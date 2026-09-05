// kernel/src/terminal.c
#include "terminal.h"

static char* vga = (char*)VGA_ADDRESS;
static int cursor_row = 0;
static int cursor_col = 0;
static unsigned char current_color = COLOR_WHITE_ON_BLACK;

static inline void outb(unsigned short port, unsigned char data) {
    __asm__ volatile ("outb %0, %1" : : "a"(data), "Nd"(port));
}

static void update_hardware_cursor() {
    unsigned short pos = cursor_row * VGA_COLS + cursor_col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void terminal_clear() {
    for (int i = 0; i < VGA_ROWS * VGA_COLS * 2; i += 2) {
        vga[i] = ' ';
        vga[i + 1] = COLOR_WHITE_ON_BLACK;
    }
    cursor_row = 0;
    cursor_col = 0;
    current_color = COLOR_WHITE_ON_BLACK;
    update_hardware_cursor();
}

void terminal_set_color(unsigned char color) {
    current_color = color;
}

void terminal_putchar_at(char c, int row, int col, unsigned char color) {
    int offset = (row * VGA_COLS + col) * 2;
    vga[offset] = c;
    vga[offset + 1] = color;
}

void terminal_putchar(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
        if (cursor_row >= VGA_ROWS) {
            terminal_scroll();
            cursor_row = VGA_ROWS - 1;
        }
        update_hardware_cursor();
        return;
    }
    if (c == '\r') {
        cursor_col = 0;
        update_hardware_cursor();
        return;
    }
    if (c == '\b') {
        if (cursor_col > 0) {
            cursor_col--;
            terminal_putchar_at(' ', cursor_row, cursor_col, current_color);
            update_hardware_cursor();
        }
        return;
    }
    terminal_putchar_at(c, cursor_row, cursor_col, current_color);
    cursor_col++;
    if (cursor_col >= VGA_COLS) {
        cursor_col = 0;
        cursor_row++;
        if (cursor_row >= VGA_ROWS) {
            terminal_scroll();
            cursor_row = VGA_ROWS - 1;
        }
    }
    update_hardware_cursor();
}

void terminal_scroll() {
    for (int row = 0; row < VGA_ROWS - 1; row++) {
        for (int col = 0; col < VGA_COLS; col++) {
            int src = ((row + 1) * VGA_COLS + col) * 2;
            int dst = (row * VGA_COLS + col) * 2;
            vga[dst] = vga[src];
            vga[dst + 1] = vga[src + 1];
        }
    }
    for (int col = 0; col < VGA_COLS; col++) {
        int offset = ((VGA_ROWS - 1) * VGA_COLS + col) * 2;
        vga[offset] = ' ';
        vga[offset + 1] = COLOR_WHITE_ON_BLACK;
    }
    update_hardware_cursor();
}

void terminal_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        terminal_putchar(str[i]);
    }
}

void terminal_write_at(const char* str, int row, int col, unsigned char color) {
    for (int i = 0; str[i] != '\0'; i++) {
        terminal_putchar_at(str[i], row, col + i, color);
    }
}

void terminal_get_cursor(int* row, int* col) {
    *row = cursor_row;
    *col = cursor_col;
}

void terminal_set_cursor(int row, int col) {
    if (row < 0) row = 0;
    if (row >= VGA_ROWS) row = VGA_ROWS - 1;
    if (col < 0) col = 0;
    if (col >= VGA_COLS) col = VGA_COLS - 1;
    cursor_row = row;
    cursor_col = col;
    update_hardware_cursor();
}