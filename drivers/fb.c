#include "kernel/uart.h"
#include "kernel/mbox.h"
#include "kernel/delays.h"
#include "kernel/fb.h"

#include "common/string.h"

#include <stdbool.h>
#include <stdarg.h>

// Declare a global instance of the structs.
FramebufferInfo fb_info;

void fb_init()
{
    wait_msec(100000);

    mbox[0] = 35 * 4;
    mbox[1] = MBOX_REQUEST;

    // Set physical width and height
    mbox[2] = 0x48003;  // Set phy wh
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = 1024;         // FrameBufferInfo.width
    mbox[6] = 768;          // FrameBufferInfo.height

    // Set virtual width and height
    mbox[7] = 0x48004;  // Set virt wh
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = 1024;        // FrameBufferInfo.virtual_width
    mbox[11] = 768;         // FrameBufferInfo.virtual_height

    // Set virtual offset
    mbox[12] = 0x48009;  // Set virt offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 10;           // FrameBufferInfo.x_offset
    mbox[16] = 10;           // FrameBufferInfo.y_offset

    // Set depth (bits per pixel)
    mbox[17] = 0x48005;  // Set depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 32;          // FrameBufferInfo.depth

    // Set pixel order (RGB or BGR)
    mbox[21] = 0x48006;  // Set pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;           // RGB, not BGR (FrameBufferInfo.pixel_order)

    // Get framebuffer pointer
    mbox[25] = 0x40001;  // Get framebuffer, gets alignment on request
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;        // FrameBufferInfo.pointer
    mbox[29] = 0;           // FrameBufferInfo.size

    // Get pitch
    mbox[30] = 0x40008;  // Get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;           // FrameBufferInfo.pitch

    mbox[34] = MBOX_TAG_LAST;

    if (mbox_call(MBOX_CH_PROP) && mbox[20] == 32 && mbox[28] != 0) {
        mbox[28] &= 0x3FFFFFFF; // Align the pointer to 4MB boundary

        // Populate the FramebufferInfo struct
        fb_info.width = mbox[5];
        fb_info.height = mbox[6];
        fb_info.virtual_width = mbox[10];
        fb_info.virtual_height = mbox[11];
        fb_info.x_offset = mbox[15];
        fb_info.y_offset = mbox[16];
        fb_info.depth = mbox[20];
        fb_info.pixel_order = mbox[24];
        fb_info.pointer = (void*)((unsigned long)mbox[28]);
        fb_info.size = mbox[29];
        fb_info.pitch = mbox[33];
        fb_info.foreground = 0xFFFFFF;
        fb_info.background = 0x000000;

        uart_puts("Framebuffer initialized.\n");
    } else {
        uart_puts("Unable to set screen resolution to 1024x768x32\n");
    }
}

// Function to put a pixel on the screen
void put_pixel(int x, int y, unsigned int attribute) 
{
    // Check if the coordinates are within bounds
    if (x < 0 || x >= fb_info.width || y < 0 || y >= fb_info.height) {
        return; // Out of bounds, do nothing
    }

    // Calculate the memory location of the pixel based on the framebuffer layout
    unsigned int pixel_offset = (y * fb_info.pitch) + (x * (fb_info.depth / 8));
    
    // Write the pixel value to the framebuffer
    *((unsigned int*)(fb_info.pointer + pixel_offset)) = attribute;
}

// Function to put a glyph (if availible) on a specified x,y cords, with a color specified in RGB (attribute)
void draw_glyph(int x, int y, const char *c_utf8, unsigned int attribute) {
    uint32_t code_point = 0;

    unsigned char c0 = c_utf8[0];
    if ((c0 & 0x80) == 0x00) {
        code_point = c0;
    } else if ((c0 & 0xE0) == 0xC0) {
        code_point = ((c0 & 0x1F) << 6) | (c_utf8[1] & 0x3F);
    } else if ((c0 & 0xF0) == 0xE0) {
        code_point = ((c0 & 0x0F) << 12) | ((c_utf8[1] & 0x3F) << 6) | (c_utf8[2] & 0x3F);
    } else if ((c0 & 0xF8) == 0xF0) {
        code_point = ((c0 & 0x07) << 18) | ((c_utf8[1] & 0x3F) << 12) |
                     ((c_utf8[2] & 0x3F) << 6) | (c_utf8[3] & 0x3F);
    } else {
        return; // Invalid UTF-8
    }

    uart_puts(c_utf8);
    sfn_t *font = (sfn_t*)&_binary_font_sfn_start;
    unsigned char *ptr, *chr, *frg;
    unsigned long o, p;
    int i, j, k, l, m, n;

    // Find the character in the SSFN font table
    for(ptr = (unsigned char*)font + font->characters_offs, chr = 0, i = 0; i < 0x110000; i++) {
        if(ptr[0] == 0xFF) {
            i += 65535; ptr++;
        } else if((ptr[0] & 0xC0) == 0xC0) { // Combined character range
            j = (((ptr[0] & 0x3F) << 8) | ptr[1]);
            i += j;
            ptr += 2;
        } else if((ptr[0] & 0xC0) == 0x80) { // Compressed character range
            j = (ptr[0] & 0x3F);
            i += j;
            ptr++;
        } else {
            if((unsigned int)i == code_point) {
                chr = ptr;
                break;
            }
            ptr += 6 + ptr[1] * (ptr[0] & 0x40 ? 6 : 5); // Skip to next character
        }
    }

    if(!chr) return;

    // Render character
    ptr = chr + 6;
    o = (unsigned long)fb_info.pointer + y * fb_info.pitch + x * 4;
    for(i = n = 0; i < chr[1]; i++, ptr += chr[0] & 0x40 ? 6 : 5) {
        if(ptr[0] == 255 && ptr[1] == 255) continue;
        frg = (unsigned char*)font + (chr[0] & 0x40 ?
            ((ptr[5] << 24) | (ptr[4] << 16) | (ptr[3] << 8) | ptr[2]) :
            ((ptr[4] << 16) | (ptr[3] << 8) | ptr[2]));

        if((frg[0] & 0xE0) != 0x80) continue;

        o += (int)(ptr[1] - n) * fb_info.pitch;
        n = ptr[1];

        k = ((frg[0] & 0x1F) + 1) << 3;
        j = frg[1] + 1;
        frg += 2;

        for(m = 1; j; j--, n++, o += fb_info.pitch) {
            for(p = o, l = 0; l < k; l++, p += 4, m <<= 1) {
                if(m > 0x80) { frg++; m = 1; }
                if(*frg & m) {
                    *((unsigned int*)p) = attribute;
                }
            }
        }
    }
}

// Cursor position
static int cursor_x = 10;
static int cursor_y = 10;

// Character cell dimensions (you can adjust based on your font size)
#define CHAR_WIDTH  8
#define CHAR_HEIGHT 16

// Function to clear a character cell (optional, for background management)
void clear_char_cell(int x, int y) {
    for (int dy = 0; dy < CHAR_HEIGHT; dy++) {
        for (int dx = 0; dx < CHAR_WIDTH; dx++) {
            put_pixel(x + dx, y + dy, 0x000000); // Black background
        }
    }
}

void kput(const char *utf8_str, unsigned int attribute) {
    while (*utf8_str) {
        unsigned char c = *utf8_str;

        int len = 1;
        if ((c & 0xE0) == 0xC0) len = 2;
        else if ((c & 0xF0) == 0xE0) len = 3;
        else if ((c & 0xF8) == 0xF0) len = 4;

        if (c == '\n') {
            cursor_x = 10;
            cursor_y += CHAR_HEIGHT;
        } else if (c == '\r') {
            cursor_x = 10;
        } else if (c == '\t') {
            cursor_x += CHAR_WIDTH * 4;
        } else {
            draw_glyph(cursor_x, cursor_y, utf8_str, attribute);
            cursor_x += CHAR_WIDTH;
        }

        utf8_str += len;

        if (cursor_x + CHAR_WIDTH >= fb_info.width) {
            cursor_x = 0;
            cursor_y += CHAR_HEIGHT;
        }
        if (cursor_y + CHAR_HEIGHT >= fb_info.height) {
            cursor_y = 0;
        }
    }
}


int printk(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;  // Skip '%'

            // Handle zero-padding and width
            int width = 0;
            bool zero_pad = false;
            if (*fmt == '0') {
                zero_pad = true;
                fmt++;
            }
            while (*fmt >= '0' && *fmt <= '9') {
                width = width * 10 + (*fmt - '0');
                fmt++;
            }

            char buf[32];
            switch (*fmt) {
                case 's': {
                    char *str = va_arg(args, char *);
                    kput(str, fb_info.foreground);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    char s[2] = {c, '\0'};
                    kput(s, fb_info.foreground);
                    break;
                }
                case 'x': {
                    unsigned int val = va_arg(args, unsigned int);
                    int len = 0;

                    // Convert to hex string in reverse
                    do {
                        int digit = val % 16;
                        buf[len++] = digit < 10 ? '0' + digit : 'a' + (digit - 10);
                        val /= 16;
                    } while (val);

                    // Pad if needed
                    while (len < width) buf[len++] = zero_pad ? '0' : ' ';
                    buf[len] = '\0';

                    // Reverse string
                    for (int i = 0; i < len / 2; i++) {
                        char tmp = buf[i];
                        buf[i] = buf[len - 1 - i];
                        buf[len - 1 - i] = tmp;
                    }

                    kput(buf, fb_info.foreground);
                    break;
                }
                case 'X': {
                    unsigned int val = va_arg(args, unsigned int);
                    int len = 0;

                    // Convert to hex string in reverse
                    do {
                        int digit = val % 16;
                        buf[len++] = digit < 10 ? '0' + digit : 'A' + (digit - 10);  // <-- UPPERCASE A
                        val /= 16;
                    } while (val);
                
                    // Pad if needed
                    while (len < width) buf[len++] = zero_pad ? '0' : ' ';
                    buf[len] = '\0';
                
                    // Reverse string
                    for (int i = 0; i < len / 2; i++) {
                        char tmp = buf[i];
                        buf[i] = buf[len - 1 - i];
                        buf[len - 1 - i] = tmp;
                    }
                
                    kput(buf, fb_info.foreground);
                    break;
                }
                case 'd': {
                    int val = va_arg(args, int);
                    char *p = buf + sizeof(buf) - 1;
                    *p = '\0';
                    bool neg = val < 0;
                    if (neg) val = -val;

                    do {
                        *--p = '0' + (val % 10);
                        val /= 10;
                    } while (val);

                    if (neg) *--p = '-';
                    kput(p, fb_info.foreground);
                    break;
                }
                default:
                    kput("�", fb_info.foreground);
            }
            fmt++;
        } else {
            char ch[2] = {*fmt++, '\0'};
            kput(ch, fb_info.foreground);
        }
    }

    va_end(args);
    return 0;
}
