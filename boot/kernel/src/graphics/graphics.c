#include <stdint.h>
#include <stddef.h>
#include "graphics.h"

static uint32_t vram_addr = 0;
static uint16_t screen_width = 0;
static uint16_t screen_height = 0;
static uint8_t screen_bpp = 0;
static uint32_t* vram = NULL;

void graphics_init_from_loader(void) {
    vram_addr = *(uint32_t*)0x9010;
    screen_width = *(uint16_t*)0x9014;
    screen_height = *(uint16_t*)0x9016;
    screen_bpp = *(uint8_t*)0x9018;
    
    if (vram_addr == 0) {
        vram_addr = 0xA0000;
        screen_width = 320;
        screen_height = 200;
        screen_bpp = 8;
    }
    
    vram = (uint32_t*)vram_addr;
    
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            uint8_t r = (x * 255) / screen_width;
            uint8_t g = (y * 255) / screen_height;
            uint8_t b = 128;
            uint32_t color = (r << 16) | (g << 8) | b;
            
            if (screen_bpp == 8) {
                color = (r + g + b) / 3;
            }
            
            put_pixel(x, y, color);
        }
    }
}

void put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) {
        return;
    }
    
    if (screen_bpp == 32) {
        vram[y * screen_width + x] = color;
    } else if (screen_bpp == 8) {
        uint8_t* vram8 = (uint8_t*)vram_addr;
        vram8[y * screen_width + x] = (uint8_t)color;
    }
}

// ============================================
// 桌面绘图辅助函数（供 desktop.c 使用）
// ============================================

int graphics_is_initialized(void) {
    return (vram != NULL);
}

uint32_t rgb_to_color(uint8_t r, uint8_t g, uint8_t b) {
    return (r << 16) | (g << 8) | b;
}

void graphics_clear(uint32_t color) {
    for (int y = 0; y < screen_height; y++) {
        for (int x = 0; x < screen_width; x++) {
            put_pixel(x, y, color);
        }
    }
}

void draw_rect(int x, int y, int w, int h, uint32_t color) {
    // 上边
    for (int i = 0; i < w; i++) {
        put_pixel(x + i, y, color);
    }
    // 下边
    for (int i = 0; i < w; i++) {
        put_pixel(x + i, y + h - 1, color);
    }
    // 左边
    for (int i = 0; i < h; i++) {
        put_pixel(x, y + i, color);
    }
    // 右边
    for (int i = 0; i < h; i++) {
        put_pixel(x + w - 1, y + i, color);
    }
}

void draw_fill_rect(int x, int y, int w, int h, uint32_t color) {
    for (int dy = 0; dy < h; dy++) {
        for (int dx = 0; dx < w; dx++) {
            put_pixel(x + dx, y + dy, color);
        }
    }
}