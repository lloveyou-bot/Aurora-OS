// kernel/src/graphics/vbe.c
#include "vbe.h"
#include "../terminal.h"

static unsigned int framebuffer_addr = 0xE0000000;
static int screen_width = 1024;
static int screen_height = 768;

int vbe_set_mode(int width, int height, int bpp) {
    (void)width; (void)height; (void)bpp;
    terminal_write("VBE mode set (hardcoded)\n");
    return 0;
}

unsigned int vbe_get_framebuffer() {
    return framebuffer_addr;
}

int vbe_get_screen_width() {
    return screen_width;
}

int vbe_get_screen_height() {
    return screen_height;
}