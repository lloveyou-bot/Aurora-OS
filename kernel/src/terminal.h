// kernel/src/terminal.h
#ifndef TERMINAL_H
#define TERMINAL_H

#define VGA_ADDRESS     0xB8000
#define VGA_COLS        80
#define VGA_ROWS        25
#define VGA_BYTES_PER_CELL 2

#define COLOR_BLACK         0x0
#define COLOR_BLUE          0x1
#define COLOR_GREEN         0x2
#define COLOR_CYAN          0x3
#define COLOR_RED           0x4
#define COLOR_MAGENTA       0x5
#define COLOR_BROWN         0x6
#define COLOR_LIGHT_GRAY    0x7
#define COLOR_DARK_GRAY     0x8
#define COLOR_LIGHT_BLUE    0x9
#define COLOR_LIGHT_GREEN   0xA
#define COLOR_LIGHT_CYAN    0xB
#define COLOR_LIGHT_RED     0xC
#define COLOR_LIGHT_MAGENTA 0xD
#define COLOR_YELLOW        0xE
#define COLOR_WHITE         0xF

#define MAKE_COLOR(fg, bg)  ((bg << 4) | fg)
#define COLOR_WHITE_ON_BLACK    MAKE_COLOR(COLOR_WHITE, COLOR_BLACK)
#define COLOR_BLACK_ON_WHITE    MAKE_COLOR(COLOR_BLACK, COLOR_WHITE)
#define COLOR_GREEN_ON_BLACK    MAKE_COLOR(COLOR_GREEN, COLOR_BLACK)
#define COLOR_RED_ON_BLACK      MAKE_COLOR(COLOR_RED, COLOR_BLACK)
#define COLOR_YELLOW_ON_BLACK   MAKE_COLOR(COLOR_YELLOW, COLOR_BLACK)
#define COLOR_CYAN_ON_BLACK     MAKE_COLOR(COLOR_CYAN, COLOR_BLACK)
#define COLOR_WHITE_ON_BLUE     MAKE_COLOR(COLOR_WHITE, COLOR_BLUE)
#define COLOR_YELLOW_ON_BLUE    MAKE_COLOR(COLOR_YELLOW, COLOR_BLUE)

void terminal_clear();
void terminal_putchar(char c);
void terminal_putchar_at(char c, int row, int col, unsigned char color);
void terminal_write(const char* str);
void terminal_write_at(const char* str, int row, int col, unsigned char color);
void terminal_scroll();
void terminal_get_cursor(int* row, int* col);
void terminal_set_cursor(int row, int col);

#endif