#ifndef FB_H
#define FB_H

// Define the FramebufferInfo struct
typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int virtual_width;
    unsigned int virtual_height;
    unsigned int foreground; // 0xRRGGBB
    unsigned int background; // 0xRRGGBB
    unsigned int x_offset;
    unsigned int y_offset;
    unsigned int depth;
    unsigned int pixel_order; // 0 = BGR, 1 = RGB
    unsigned char *pointer;
    unsigned int size;
    unsigned int pitch;
} FramebufferInfo;


typedef struct {
    unsigned char  magic[4];
    unsigned int   size;
    unsigned char  type;
    unsigned char  features;
    unsigned char  width;
    unsigned char  height;
    unsigned char  baseline;
    unsigned char  underline;
    unsigned short fragments_offs;
    unsigned int   characters_offs;
    unsigned int   ligature_offs;
    unsigned int   kerning_offs;
    unsigned int   cmap_offs;
} __attribute__((packed)) sfn_t;
extern volatile unsigned char _binary_font_sfn_start[];

void fb_init();
void kput(const char *utf8_str, unsigned int attribute);
void put_pixel(int x, int y, unsigned int attribute);
void clear_screen(unsigned int attribute);
void draw_glyph(int x, int y, const char *c_utf8, unsigned int attribute);
int printk(const char *fmt, ...);

#endif // FB_H