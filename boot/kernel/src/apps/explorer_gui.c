// kernel/src/apps/explorer_gui.c
#include "../gui/window.h"
#include "../fs.h"
#include "../font/hzk16.h"
#include "../graphics/graphics.h"

static int explorer_win = -1;
static int selected = 0;

static void draw_file_item(int x, int y, const char* name, int is_dir, int selected) {
    unsigned int color = selected ? rgb_to_color(0, 80, 200) : rgb_to_color(30, 30, 30);
    unsigned int text_color = selected ? rgb_to_color(255, 255, 255) : rgb_to_color(200, 200, 200);
    
    draw_fill_rect(x, y, 80, 80, color);
    draw_rect(x, y, 80, 80, rgb_to_color(80, 80, 80));
    
    if (is_dir) {
        draw_fill_rect(x + 20, y + 10, 40, 30, rgb_to_color(200, 200, 0));
        draw_fill_rect(x + 15, y + 20, 50, 20, rgb_to_color(200, 200, 100));
    } else {
        draw_fill_rect(x + 20, y + 10, 40, 40, rgb_to_color(200, 200, 255));
    }
    
    hzk_draw_string(x + 10, y + 55, text_color, 0xFFFFFFFF, name);
}

static void explorer_draw(int win_id) {
    Window* win = &wm.wins[win_id];
    if (!win->active) return;
    
    int start_x = win->x + 10;
    int start_y = win->y + 30;
    int cols = (win->w - 20) / 90;
    
    int count = 0;
    file_entry_t* files[32];
    for (int i = 0; i < fs_get_file_count() && count < 32; i++) {
        file_entry_t* f = fs_get_file(i);
        if (f && f->parent != -2) {
            files[count++] = f;
        }
    }
    
    for (int i = 0; i < count; i++) {
        int row = i / cols;
        int col = i % cols;
        int px = start_x + col * 90;
        int py = start_y + row * 90;
        
        draw_file_item(px, py, files[i]->name, files[i]->type == 0, i == selected);
    }
}

static void explorer_on_key(int win_id, char key) {
    (void)win_id;
    if (key == 'w' && selected > 0) selected--;
    if (key == 's') selected++;
    if (key == '\n') {
        // 打开选中文件
    }
    wm_draw_window(win_id);
}

int explorer_gui_start() {
    if (explorer_win >= 0) {
        wm_set_active(explorer_win);
        return explorer_win;
    }
    
    explorer_win = wm_create_window(50, 50, 600, 400, "文件管理器");
    if (explorer_win < 0) return -1;
    
    wm.wins[explorer_win].draw = explorer_draw;
    wm.wins[explorer_win].on_key = explorer_on_key;
    wm_set_active(explorer_win);
    
    return explorer_win;
}