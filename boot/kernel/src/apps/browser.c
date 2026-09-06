// kernel/src/apps/browser.c
#include "../gui/window.h"
#include "../font/hzk16.h"
#include "../graphics/graphics.h"

static int browser_win = -1;
static char url[256] = "about:blank";

static void render_html(int win_id, const char* html) {
    Window* win = &wm.wins[win_id];
    int x = win->x + 10, y = win->y + 40;
    
    const char* p = html;
    while (*p) {
        if (*p == '<') {
            p++;
            if (*p == '/') {
                p++;
                while (*p && *p != '>') p++;
                p++;
                continue;
            }
            if (*p == 'h' && *(p+1) == '1') {
                p += 3;
                while (*p && *p != '<') {
                    hzk_draw_string(x, y, rgb_to_color(255, 255, 0), 0xFFFFFFFF, p);
                    p++;
                }
                y += 30;
            } else if (*p == 'p') {
                p += 2;
                while (*p && *p != '<') {
                    hzk_draw_string(x, y, rgb_to_color(200, 200, 200), 0xFFFFFFFF, p);
                    p++;
                }
                y += 20;
            } else {
                while (*p && *p != '>') p++;
                p++;
            }
        } else {
            if (*p == '\n') y += 20;
            p++;
        }
    }
}

static void browser_draw(int win_id) {
    Window* win = &wm.wins[win_id];
    
    draw_fill_rect(win->x + 10, win->y + 22, win->w - 20, 20, rgb_to_color(50, 50, 50));
    draw_rect(win->x + 10, win->y + 22, win->w - 20, 20, rgb_to_color(80, 80, 80));
    hzk_draw_string(win->x + 15, win->y + 25, rgb_to_color(255, 255, 255), 0xFFFFFFFF, url);
    
    const char* test_html = "<h1>Hello Aurora!</h1><p>欢迎使用 Aurora OS 浏览器</p>";
    render_html(win_id, test_html);
}

int browser_start() {
    if (browser_win >= 0) {
        wm_set_active(browser_win);
        return browser_win;
    }
    
    browser_win = wm_create_window(100, 60, 700, 500, "Aurora Browser");
    if (browser_win < 0) return -1;
    
    wm.wins[browser_win].draw = browser_draw;
    wm_set_active(browser_win);
    
    return browser_win;
}