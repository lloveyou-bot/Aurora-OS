// kernel/src/font/hzk16.c
#include <stddef.h>
#include "hzk16.h"
#include "../graphics/graphics.h"

HzkFont hzk_font;

void hzk_init(unsigned char* font_data, int size) {
    hzk_font.data = font_data;
    hzk_font.size = size;
    hzk_font.loaded = (font_data != NULL);
}

int hzk_draw_string(int x, int y, unsigned int color, unsigned int bg, const char* str) {
    (void)x; (void)y; (void)color; (void)bg;
    // 暂时只显示英文
    char* video = (char*)0xB8000;
    int i = 0;
    while (str[i] != '\0' && i < 80) {
        video[i * 2] = str[i];
        video[i * 2 + 1] = 0x0F;
        i++;
    }
    return 0;
}