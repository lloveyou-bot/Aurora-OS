#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

void graphics_init_from_loader(void);
void put_pixel(int x, int y, uint32_t color);

// 桌面绘图辅助函数
int graphics_is_initialized(void);
uint32_t rgb_to_color(uint8_t r, uint8_t g, uint8_t b);
void graphics_clear(uint32_t color);
void draw_rect(int x, int y, int w, int h, uint32_t color);
void draw_fill_rect(int x, int y, int w, int h, uint32_t color);

#endif