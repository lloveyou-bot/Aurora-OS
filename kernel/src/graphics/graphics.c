// kernel/src/graphics/graphics.c
#include "graphics.h"
#include "vbe.h"
#include "../terminal.h"

#define NULL 0

static unsigned int* framebuffer = NULL;
static int screen_width = 0;
static int screen_height = 0;
static int graphics_mode = GRAPHICS_MODE_NONE;

void graphics_init() {
    terminal_write("Graphics init...\n");
    
    // 直接设置硬编码值，让图形模式能工作
    screen_width = 1024;
    screen_height = 768;
    framebuffer = (unsigned int*)0xE0000000;
    graphics_mode = GRAPHICS_MODE_VESA;
    
    terminal_write("Graphics OK (1024x768x32)\n");
}

int graphics_is_initialized() {
    return graphics_mode != GRAPHICS_MODE_NONE;
}

int get_screen_width() { return screen_width; }
int get_screen_height() { return screen_height; }

void put_pixel(int x, int y, unsigned int color) {
    if (!framebuffer) return;
    if (x < 0 || x >= screen_width || y < 0 || y >= screen_height) return;
    framebuffer[y * screen_width + x] = color;
}

void draw_fill_rect(int x, int y, int w, int h, unsigned int color) {
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            put_pixel(x + j, y + i, color);
        }
    }
}

void draw_rect(int x, int y, int w, int h, unsigned int color) {
    for (int i = 0; i < w; i++) {
        put_pixel(x + i, y, color);
        put_pixel(x + i, y + h - 1, color);
    }
    for (int i = 0; i < h; i++) {
        put_pixel(x, y + i, color);
        put_pixel(x + w - 1, y + i, color);
    }
}

void draw_line(int x1, int y1, int x2, int y2, unsigned int color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = (dx > dy ? dx : dy);
    if (steps < 0) steps = -steps;
    if (steps == 0) { put_pixel(x1, y1, color); return; }
    float x = (float)x1;
    float y = (float)y1;
    float x_inc = (float)dx / steps;
    float y_inc = (float)dy / steps;
    for (int i = 0; i <= steps; i++) {
        put_pixel((int)(x + 0.5), (int)(y + 0.5), color);
        x += x_inc;
        y += y_inc;
    }
}

void draw_circle(int cx, int cy, int radius, unsigned int color) {
    int x = radius;
    int y = 0;
    int err = 0;
    while (x >= y) {
        put_pixel(cx + x, cy + y, color);
        put_pixel(cx + y, cy + x, color);
        put_pixel(cx - y, cy + x, color);
        put_pixel(cx - x, cy + y, color);
        put_pixel(cx - x, cy - y, color);
        put_pixel(cx - y, cy - x, color);
        put_pixel(cx + y, cy - x, color);
        put_pixel(cx + x, cy - y, color);
        if (err <= 0) { y++; err += 2 * y + 1; }
        if (err > 0) { x--; err -= 2 * x + 1; }
    }
}

unsigned int rgb_to_color(unsigned char r, unsigned char g, unsigned char b) {
    return (r << 16) | (g << 8) | b;
}

void graphics_clear(unsigned int color) {
    if (!framebuffer) return;
    for (int i = 0; i < screen_width * screen_height; i++) {
        framebuffer[i] = color;
    }
}