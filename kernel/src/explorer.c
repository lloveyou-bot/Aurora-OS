// kernel/src/explorer.c - 显示真实文件系统
#include "terminal.h"
#include "fs.h"
#include "explorer.h"
#include "shell.h"
#define NULL 0

#define WIN_ROW    2
#define WIN_COL    4
#define WIN_WIDTH  72
#define WIN_HEIGHT 20

static int selected_item = 0;
static int scroll_offset = 0;
static int running = 1;

// 获取当前目录的文件列表
static void get_file_list(const char** names, int* types, int* count) {
    int cnt = 0;
    for (int i = 0; i < fs_get_file_count() && cnt < 32; i++) {
        file_entry_t* f = fs_get_file(i);
        if (f == NULL) continue;
        // 检查是否在当前目录
        // 简化：显示所有文件
        names[cnt] = f->name;
        types[cnt] = f->type;
        cnt++;
    }
    *count = cnt;
}

// 绘制圆角矩形边框
static void draw_rounded_box(int row, int col, int width, int height, unsigned char border_color, unsigned char fill_color) {
    terminal_putchar_at('+', row, col, border_color);
    terminal_putchar_at('+', row, col + width - 1, border_color);
    terminal_putchar_at('+', row + height - 1, col, border_color);
    terminal_putchar_at('+', row + height - 1, col + width - 1, border_color);
    
    for (int i = col + 1; i < col + width - 1; i++) {
        terminal_putchar_at('-', row, i, border_color);
    }
    
    for (int i = row + 1; i < row + height - 1; i++) {
        terminal_putchar_at('|', i, col, border_color);
        terminal_putchar_at('|', i, col + width - 1, border_color);
    }
    
    for (int i = col + 1; i < col + width - 1; i++) {
        terminal_putchar_at('-', row + height - 1, i, border_color);
    }
    
    for (int r = row + 1; r < row + height - 1; r++) {
        for (int c = col + 1; c < col + width - 1; c++) {
            terminal_putchar_at(' ', r, c, fill_color);
        }
    }
}

// 绘制标题栏
static void draw_titlebar(int row, int col, int width, const char* title, unsigned char bg_color, unsigned char text_color) {
    for (int i = 0; i < width; i++) {
        terminal_putchar_at(' ', row, col + i, bg_color);
    }
    
    terminal_putchar_at('[', row, col + 1, text_color);
    terminal_putchar_at(']', row, col + 2, text_color);
    terminal_putchar_at(' ', row, col + 3, bg_color);
    
    int title_len = 0;
    while (title[title_len]) title_len++;
    int start = col + (width - title_len) / 2;
    for (int i = 0; title[i]; i++) {
        terminal_putchar_at(title[i], row, start + i, text_color);
    }
    
    terminal_putchar_at('[', row, col + width - 6, text_color);
    terminal_putchar_at('_', row, col + width - 5, text_color);
    terminal_putchar_at(']', row, col + width - 4, text_color);
    terminal_putchar_at('[', row, col + width - 3, text_color);
    terminal_putchar_at('X', row, col + width - 2, MAKE_COLOR(COLOR_RED, COLOR_BLACK));
    terminal_putchar_at(']', row, col + width - 1, text_color);
}

// 绘制文件列表
static void draw_file_list() {
    int start_row = WIN_ROW + 3;
    int max_display = WIN_HEIGHT - 5;
    
    const char* names[32];
    int types[32];
    int count = 0;
    get_file_list(names, types, &count);
    
    for (int i = 0; i < max_display && i < count; i++) {
        int idx = i + scroll_offset;
        if (idx >= count) break;
        
        int row = start_row + i;
        int col = WIN_COL + 2;
        unsigned char color = (i == selected_item) ? MAKE_COLOR(COLOR_BLACK, COLOR_WHITE) : MAKE_COLOR(COLOR_WHITE, COLOR_BLACK);
        
        for (int c = col; c < WIN_COL + WIN_WIDTH - 2; c++) {
            terminal_putchar_at(' ', row, c, color);
        }
        
        if (types[idx] == 0) {
            terminal_write_at("[DIR] ", row, col, color);
        } else {
            terminal_write_at("[FILE]", row, col, color);
        }
        
        terminal_write_at(names[idx], row, col + 6, color);
    }
}

// 绘制底部状态栏
static void draw_statusbar(int row, int col, int width) {
    for (int i = 0; i < width; i++) {
        terminal_putchar_at(' ', row, col + i, MAKE_COLOR(COLOR_WHITE, COLOR_BLUE));
    }
    terminal_write_at("W/S Navigate  Enter=Open  ESC=Exit", row, col + 2, MAKE_COLOR(COLOR_YELLOW, COLOR_BLUE));
}

// 绘制地址栏
static void draw_addressbar(int row, int col, int width) {
    for (int i = 0; i < width; i++) {
        terminal_putchar_at(' ', row, col + i, MAKE_COLOR(COLOR_BLACK, COLOR_WHITE));
    }
    terminal_write_at(" Address: /", row, col + 2, MAKE_COLOR(COLOR_BLACK, COLOR_WHITE));
}

// 绘制主窗口
void explorer_draw() {
    terminal_clear();
    
    draw_rounded_box(WIN_ROW, WIN_COL, WIN_WIDTH, WIN_HEIGHT, MAKE_COLOR(COLOR_CYAN, COLOR_BLACK), MAKE_COLOR(COLOR_WHITE, COLOR_BLACK));
    draw_titlebar(WIN_ROW, WIN_COL, WIN_WIDTH, "Explorer", MAKE_COLOR(COLOR_WHITE, COLOR_BLUE), MAKE_COLOR(COLOR_YELLOW, COLOR_BLUE));
    draw_addressbar(WIN_ROW + 1, WIN_COL + 1, WIN_WIDTH - 2);
    draw_file_list();
    draw_statusbar(WIN_ROW + WIN_HEIGHT - 1, WIN_COL, WIN_WIDTH);
}

void explorer_init() {
    selected_item = 0;
    scroll_offset = 0;
    running = 1;
    explorer_draw();
}

int explorer_process_key(char c) {
    if (c == '\n' || c == '\r') {
        const char* names[32];
        int types[32];
        int count = 0;
        get_file_list(names, types, &count);
        if (selected_item + scroll_offset < count) {
            terminal_write_at("Opening: ", WIN_ROW + WIN_HEIGHT + 1, 2, MAKE_COLOR(COLOR_WHITE, COLOR_BLACK));
            terminal_write_at(names[selected_item + scroll_offset], WIN_ROW + WIN_HEIGHT + 1, 12, MAKE_COLOR(COLOR_WHITE, COLOR_BLACK));
        }
        return 0;
    }
    
    if (c == 27) {
        running = 0;
        return 1;
    }
    
    const char* names[32];
    int types[32];
    int count = 0;
    get_file_list(names, types, &count);
    
    if (c == 'w' || c == 'W') {
        if (selected_item > 0) {
            selected_item--;
            if (selected_item < scroll_offset) {
                scroll_offset = selected_item;
            }
            explorer_draw();
        }
        return 0;
    }
    if (c == 's' || c == 'S') {
        if (selected_item < count - 1) {
            selected_item++;
            if (selected_item >= scroll_offset + (WIN_HEIGHT - 5)) {
                scroll_offset = selected_item - (WIN_HEIGHT - 5) + 1;
            }
            explorer_draw();
        }
        return 0;
    }
    
    return 0;
}

void explorer_run() {}