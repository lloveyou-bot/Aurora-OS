// kernel/src/desktop/desktop.c
#include <stddef.h>
#include "desktop.h"
#include "../graphics/graphics.h"
#include "../gui/window.h"

void desktop_init() {
    // 初始化窗口管理器
    extern void wm_init();
    wm_init();
}

void desktop_draw() {
    if (!graphics_is_initialized()) {
        return;
    }

    // 清屏
    graphics_clear(rgb_to_color(20, 20, 60));

    // 渐变壁纸
    for (int y = 0; y < 768; y++) {
        for (int x = 0; x < 1024; x++) {
            unsigned char r = (x * 255 / 1024);
            unsigned char g = (y * 255 / 768);
            unsigned char b = 100;
            put_pixel(x, y, rgb_to_color(r, g, b));
        }
    }

    // 绘制任务栏
    draw_fill_rect(0, 728, 1024, 40, rgb_to_color(40, 40, 40));
    draw_fill_rect(5, 732, 60, 30, rgb_to_color(0, 80, 200));
    draw_fill_rect(70, 732, 80, 30, rgb_to_color(0, 100, 0));
    draw_fill_rect(155, 732, 80, 30, rgb_to_color(100, 0, 0));

    // 绘制示例窗口
    // 窗口1：红色主题
    draw_fill_rect(80, 60, 300, 200, rgb_to_color(30, 30, 30));
    draw_rect(80, 60, 300, 200, rgb_to_color(200, 50, 50));
    draw_fill_rect(80, 60, 300, 20, rgb_to_color(200, 50, 50));

    // 窗口2：绿色主题
    draw_fill_rect(450, 80, 300, 200, rgb_to_color(30, 30, 30));
    draw_rect(450, 80, 300, 200, rgb_to_color(50, 200, 50));
    draw_fill_rect(450, 80, 300, 20, rgb_to_color(50, 200, 50));

    // 窗口3：蓝色主题
    draw_fill_rect(250, 300, 300, 200, rgb_to_color(30, 30, 30));
    draw_rect(250, 300, 300, 200, rgb_to_color(50, 50, 200));
    draw_fill_rect(250, 300, 300, 20, rgb_to_color(50, 50, 200));

    // 桌面图标
    draw_fill_rect(20, 20, 40, 40, rgb_to_color(200, 200, 0));
    draw_fill_rect(30, 50, 20, 20, rgb_to_color(200, 200, 200));

    draw_fill_rect(20, 80, 40, 40, rgb_to_color(0, 200, 200));
    draw_fill_rect(30, 110, 20, 20, rgb_to_color(200, 200, 200));

    draw_fill_rect(20, 140, 40, 40, rgb_to_color(200, 0, 200));
    draw_fill_rect(30, 170, 20, 20, rgb_to_color(200, 200, 200));
}
