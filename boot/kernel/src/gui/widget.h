// kernel/src/gui/widget.h
#ifndef WIDGET_H
#define WIDGET_H

#include "../graphics/graphics.h"

typedef struct {
    int type;
    int x, y, w, h;
    unsigned int bg_color;
    unsigned int text_color;
    char text[64];
    int enabled;
} Widget;

void widget_draw(Widget* w);
void widget_handle_click(Widget* w, int x, int y);

#endif
