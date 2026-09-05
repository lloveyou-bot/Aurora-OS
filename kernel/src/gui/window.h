#ifndef WINDOW_H
#define WINDOW_H

#define MAX_WINDOWS 16
#define MAX_TITLE_LEN 64

typedef struct {
    int x, y, w, h;
    int z_order;
    int active;
    char title[MAX_TITLE_LEN];
    unsigned int bg_color;
    unsigned int title_color;
    void (*draw)(int win_id);
    void (*on_key)(int win_id, char key);
    void (*on_mouse)(int win_id, int x, int y, int btn);
    void* data;
} Window;

typedef struct {
    Window wins[MAX_WINDOWS];
    int count;
    int active_id;
    int drag_win;
    int drag_ox, drag_oy;
    int mouse_x, mouse_y;
    int mouse_btn;
} WindowManager;

extern WindowManager wm;

void wm_init();
int wm_create_window(int x, int y, int w, int h, const char* title);
void wm_close_window(int id);
void wm_set_active(int id);
void wm_draw();
void wm_draw_window(int id);
void wm_draw_titlebar(int id);
void wm_draw_client(int id);
void wm_handle_key(char key);
void wm_handle_mouse(int x, int y, int btn);

#endif