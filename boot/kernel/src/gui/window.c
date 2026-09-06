// kernel/src/gui/window.c
#include <stddef.h>
#include "window.h"
#include "../graphics/graphics.h"
#include "../font/hzk16.h"

WindowManager wm;

void wm_init() {
    wm.count = 0;
    wm.active_id = -1;
    wm.drag_win = -1;
    wm.mouse_x = 0;
    wm.mouse_y = 0;
    wm.mouse_btn = 0;
}

int wm_create_window(int x, int y, int w, int h, const char* title) {
    if (wm.count >= MAX_WINDOWS) return -1;
    
    Window* win = &wm.wins[wm.count];
    win->x = x;
    win->y = y;
    win->w = w;
    win->h = h;
    win->z_order = wm.count;
    win->active = 1;
    win->bg_color = rgb_to_color(30, 30, 30);
    win->title_color = rgb_to_color(255, 255, 255);
    win->draw = NULL;
    win->on_key = NULL;
    win->on_mouse = NULL;
    win->data = NULL;
    
    int i = 0;
    while (title[i] && i < MAX_TITLE_LEN - 1) {
        win->title[i] = title[i];
        i++;
    }
    win->title[i] = '\0';
    
    wm.active_id = wm.count;
    wm.count++;
    return wm.count - 1;
}

void wm_close_window(int id) {
    if (id < 0 || id >= wm.count) return;
    wm.wins[id].active = 0;
}

void wm_set_active(int id) {
    if (id >= 0 && id < wm.count && wm.wins[id].active) {
        wm.active_id = id;
        wm.wins[id].z_order = wm.count - 1;
    }
}

void wm_draw_titlebar(int id) {
    Window* win = &wm.wins[id];
    if (!win->active) return;
    
    unsigned int color = win->active ? rgb_to_color(0, 80, 200) : rgb_to_color(80, 80, 80);
    draw_fill_rect(win->x, win->y, win->w, 20, color);
    
    hzk_draw_string(win->x + 5, win->y + 2, rgb_to_color(255, 255, 255), 0xFFFFFFFF, win->title);
    
    draw_rect(win->x + win->w - 20, win->y + 2, 16, 16, rgb_to_color(255, 0, 0));
    hzk_draw_string(win->x + win->w - 16, win->y + 3, rgb_to_color(255, 255, 255), 0xFFFFFFFF, "X");
}

void wm_draw_client(int id) {
    Window* win = &wm.wins[id];
    if (!win->active) return;
    
    draw_fill_rect(win->x + 2, win->y + 22, win->w - 4, win->h - 24, win->bg_color);
    
    if (win->draw) {
        win->draw(id);
    }
}

void wm_draw_window(int id) {
    Window* win = &wm.wins[id];
    if (!win->active) return;
    
    draw_rect(win->x, win->y, win->w, win->h, rgb_to_color(80, 80, 80));
    wm_draw_titlebar(id);
    wm_draw_client(id);
}

void wm_draw() {
    for (int z = 0; z < wm.count; z++) {
        for (int i = 0; i < wm.count; i++) {
            if (wm.wins[i].active && wm.wins[i].z_order == z) {
                wm_draw_window(i);
            }
        }
    }
}

void wm_handle_key(char key) {
    if (wm.active_id >= 0 && wm.wins[wm.active_id].on_key) {
        wm.wins[wm.active_id].on_key(wm.active_id, key);
    }
}

void wm_handle_mouse(int x, int y, int btn) {
    wm.mouse_x = x;
    wm.mouse_y = y;
    wm.mouse_btn = btn;
    
    for (int i = wm.count - 1; i >= 0; i--) {
        Window* win = &wm.wins[i];
        if (!win->active) continue;
        if (x >= win->x && x < win->x + win->w &&
            y >= win->y && y < win->y + win->h) {
            if (btn) {
                wm_set_active(i);
                if (y < win->y + 20) {
                    wm.drag_win = i;
                    wm.drag_ox = x - win->x;
                    wm.drag_oy = y - win->y;
                }
                if (win->on_mouse) {
                    win->on_mouse(i, x - win->x, y - win->y, btn);
                }
            }
            return;
        }
    }
    
    if (wm.drag_win >= 0 && btn) {
        Window* win = &wm.wins[wm.drag_win];
        win->x = x - wm.drag_ox;
        win->y = y - wm.drag_oy;
    } else {
        wm.drag_win = -1;
    }
}