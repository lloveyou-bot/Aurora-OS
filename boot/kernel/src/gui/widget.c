// kernel/src/gui/widget.c
#include "widget.h"
#include "../graphics/graphics.h"

void widget_draw(Widget* w) {
    if (!w->enabled) return;

    // 绘制按钮边框
    draw_rect(w->x, w->y, w->w, w->h, rgb_to_color(100, 100, 100));
    // 填充背景
    draw_fill_rect(w->x + 2, w->y + 2, w->w - 4, w->h - 4, w->bg_color);
}

void widget_handle_click(Widget* w, int x, int y) {
    if (!w->enabled) return;
    if (x >= w->x && x < w->x + w->w &&
        y >= w->y && y < w->y + w->h) {
        // 点击事件处理
        w->enabled = !w->enabled;
    }
}
