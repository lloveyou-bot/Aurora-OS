// kernel/src/graphics/graphics.h
#ifndef GRAPHICS_H
#define GRAPHICS_H

#define GRAPHICS_MODE_NONE   0
#define GRAPHICS_MODE_VESA   1

void graphics_init();
int graphics_is_initialized();

void put_pixel(int x, int y, unsigned int color);
unsigned int get_pixel(int x, int y);
void draw_rect(int x, int y, int w, int h, unsigned int color);
void draw_fill_rect(int x, int y, int w, int h, unsigned int color);
void draw_line(int x1, int y1, int x2, int y2, unsigned int color);
void draw_circle(int cx, int cy, int radius, unsigned int color);

unsigned int rgb_to_color(unsigned char r, unsigned char g, unsigned char b);
void color_to_rgb(unsigned int color, unsigned char* r, unsigned char* g, unsigned char* b);

int get_screen_width();
int get_screen_height();
void graphics_clear(unsigned int color);

#endif