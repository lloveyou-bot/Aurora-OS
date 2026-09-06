// kernel/src/graphics/sheet.c
#include <stddef.h>
#include "sheet.h"
#include "../graphics/graphics.h"

struct SHTCTL* shtctl_init(unsigned char* vram, int xsize, int ysize) {
    struct SHTCTL* ctl = (struct SHTCTL*)((unsigned int)0x100000 + 0x1000);
    ctl->vram = vram;
    ctl->xsize = xsize;
    ctl->ysize = ysize;
    ctl->top = -1;
    for (int i = 0; i < MAX_SHEETS; i++) {
        ctl->sheets0[i].flags = 0;
        ctl->sheets[i] = NULL;
    }
    return ctl;
}

struct SHEET* sheet_alloc(struct SHTCTL* ctl) {
    for (int i = 0; i < MAX_SHEETS; i++) {
        if (ctl->sheets0[i].flags == 0) {
            struct SHEET* sht = &ctl->sheets0[i];
            sht->flags = 1;
            sht->height = -1;
            return sht;
        }
    }
    return NULL;
}

void sheet_setbuf(struct SHEET* sht, unsigned char* buf, int bxsize, int bysize, int col_inv) {
    sht->buf = buf;
    sht->bxsize = bxsize;
    sht->bysize = bysize;
    sht->col_inv = col_inv;
    sht->vx0 = 0;
    sht->vy0 = 0;
}

void sheet_updown(struct SHEET* sht, int height) {
    struct SHTCTL* ctl = (struct SHTCTL*)((unsigned int)0x100000 + 0x1000);
    if (height > ctl->top + 1) height = ctl->top + 1;
    if (height < -1) height = -1;
    sht->height = height;
    
    for (int i = 0; i < MAX_SHEETS; i++) {
        if (ctl->sheets[i] == sht) {
            for (int j = i; j < ctl->top; j++) {
                ctl->sheets[j] = ctl->sheets[j + 1];
            }
            break;
        }
    }
    if (height >= 0) {
        for (int i = ctl->top; i >= height; i--) {
            ctl->sheets[i + 1] = ctl->sheets[i];
        }
        ctl->sheets[height] = sht;
        if (ctl->top < height) ctl->top = height;
    }
    sheet_refresh(ctl);
}

void sheet_refresh(struct SHTCTL* ctl) {
    for (int h = 0; h <= ctl->top; h++) {
        struct SHEET* sht = ctl->sheets[h];
        if (sht == NULL) continue;
        for (int by = 0; by < sht->bysize; by++) {
            for (int bx = 0; bx < sht->bxsize; bx++) {
                int sx = sht->vx0 + bx;
                int sy = sht->vy0 + by;
                unsigned char c = sht->buf[by * sht->bxsize + bx];
                if (c != sht->col_inv) {
                    put_pixel(sx, sy, c);
                }
            }
        }
    }
}

void sheet_slide(struct SHEET* sht, int vx0, int vy0) {
    sht->vx0 = vx0;
    sht->vy0 = vy0;
    struct SHTCTL* ctl = (struct SHTCTL*)((unsigned int)0x100000 + 0x1000);
    sheet_refresh(ctl);
}