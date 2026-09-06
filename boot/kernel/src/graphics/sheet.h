#ifndef SHEET_H
#define SHEET_H

#define MAX_SHEETS 256

struct SHEET {
    unsigned char* buf;
    int bxsize, bysize;
    int vx0, vy0;
    int col_inv;
    int height;
    int flags;
};

struct SHTCTL {
    unsigned char* vram;
    int xsize, ysize;
    int top;
    struct SHEET* sheets[MAX_SHEETS];
    struct SHEET sheets0[MAX_SHEETS];
};

struct SHTCTL* shtctl_init(unsigned char* vram, int xsize, int ysize);
struct SHEET* sheet_alloc(struct SHTCTL* ctl);
void sheet_setbuf(struct SHEET* sht, unsigned char* buf, int bxsize, int bysize, int col_inv);
void sheet_updown(struct SHEET* sht, int height);
void sheet_refresh(struct SHTCTL* ctl);
void sheet_slide(struct SHEET* sht, int vx0, int vy0);

#endif