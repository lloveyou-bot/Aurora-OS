// kernel/src/graphics/vbe.h
#ifndef VBE_H
#define VBE_H

typedef struct {
    char     signature[4];
    short    version;
    long     oem_string_ptr;
    char     capabilities[4];
    long     video_mode_ptr;
    short    total_memory;
    short    oem_software_rev;
    long     oem_vendor_name_ptr;
    long     oem_product_name_ptr;
    long     oem_product_rev_ptr;
    char     reserved[222];
    char     oem_data[256];
} __attribute__((packed)) VbeInfoBlock;

typedef struct {
    short    mode_attributes;
    char     win_a_attributes;
    char     win_b_attributes;
    short    win_granularity;
    short    win_size;
    short    win_a_segment;
    short    win_b_segment;
    long     win_func_ptr;
    short    bytes_per_scanline;
    short    x_resolution;
    short    y_resolution;
    char     x_char_size;
    char     y_char_size;
    char     planes;
    char     bits_per_pixel;
    char     banks;
    char     memory_model;
    char     bank_size;
    char     image_pages;
    char     reserved0;
    char     red_mask_size;
    char     red_field_position;
    char     green_mask_size;
    char     green_field_position;
    char     blue_mask_size;
    char     blue_field_position;
    char     rsvd_mask_size;
    char     rsvd_field_position;
    char     direct_color_mode_info;
    long     phys_base_ptr;
    long     reserved1;
    short    reserved2;
} __attribute__((packed)) VbeModeInfo;

int vbe_set_mode(int width, int height, int bpp);
unsigned int vbe_get_framebuffer();
int vbe_get_screen_width();
int vbe_get_screen_height();

#endif