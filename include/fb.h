#ifndef FB_H
#define FB_H

// Define the FramebufferInfo struct
typedef struct {
    unsigned int width;
    unsigned int height;
    unsigned int virtual_width;
    unsigned int virtual_height;
    unsigned int x_offset;
    unsigned int y_offset;
    unsigned int depth;
    unsigned int pixel_order; // 0 for BGR, 1 for RGB
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
void put_pixel(int x, int y, unsigned int attribute);
void putchar(int x, int y, const char *c_utf8, unsigned int attribute);

#endif // FB_H