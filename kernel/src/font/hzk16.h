#ifndef HZK16_H
#define HZK16_H

#define HZK16_BYTES_PER_CHAR 32

typedef struct {
    unsigned char* data;
    int size;
    int loaded;
} HzkFont;

extern HzkFont hzk_font;

void hzk_init(unsigned char* font_data, int size);
int hzk_draw_char(int x, int y, unsigned int color, unsigned int bg, unsigned short gb2312);
int hzk_draw_string(int x, int y, unsigned int color, unsigned int bg, const char* str);
int is_chinese_char(unsigned char c);

#endif