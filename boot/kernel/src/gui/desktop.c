// kernel/src/gui/desktop.c
#include <stddef.h>
#include "desktop.h"
#include "window.h"
#include "../graphics/graphics.h"
#include "../font/hzk16.h"

static void draw_wallpaper() {
    // 渐变背景（不用壁纸文件）
    for (int y = 0; y < 768; y++) {
        for (int x = 0; x < 1024; x++) {
            unsigned char r = (x * 255 / 1024);
            unsigned char g = (y * 255 / 768);
            unsigned char b = 100;
            put_pixel(x, y, rgb_to_color(r, g, b));
        }
    }
}

static void draw_taskbar() {
    draw_fill_rect(0, 728, 1024, 40, rgb_to_color(40, 40, 40));
    draw_fill_rect(5, 732, 60, 30, rgb_to_color(0, 80, 200));
    hzk_draw_string(12, 736, rgb_to_color(255, 255, 255), 0xFFFFFFFF, "开始");
    
    for (int i = 0; i < wm.count; i++) {
        if (wm.wins[i].active) {
            int bx = 70 + i * 90;
            draw_fill_rect(bx, 732, 80, 30, i == wm.active_id ? rgb_to_color(60, 60, 60) : rgb_to_color(50, 50, 50));
            hzk_draw_string(bx + 5, 736, rgb_to_color(255, 255, 255), 0xFFFFFFFF, wm.wins[i].title);
        }
    }
}

void desktop_draw() {
    draw_wallpaper();
    wm_draw();
    draw_taskbar();
}

void desktop_init() {
    wm_init();
}