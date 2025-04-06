#include "uart.h"
#include "mbox.h"
#include "delays.h"
#include "fb.h"
#include <stdbool.h>

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

// Declare a global instance of the structs.
FramebufferInfo fb_info;
sfn_t ssfn_font;

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
    mbox[15] = 0;           // FrameBufferInfo.x_offset
    mbox[16] = 0;           // FrameBufferInfo.y_offset

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


        ssfn_font.width = 32;
        ssfn_font.height = 32;
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

void putchar(int x, int y, char c, unsigned int attribute) {
    sfn_t *font = (sfn_t*)&_binary_font_sfn_start;
    unsigned char *ptr, *chr, *frg;
    unsigned int code_point;
    unsigned long o, p;
    int i, j, k, l, m, n;

    // Handle UTF-8 to Unicode code point conversion if needed
    if((c & 0x80) != 0) {
        if((c & 0x20) == 0) { // 2-byte UTF-8
            code_point = ((c & 0x1F) << 6) | (c & 0x3F);
        } else if((c & 0x10) == 0) { // 3-byte UTF-8
            code_point = ((c & 0xF) << 12) | ((c & 0x3F) << 6) | (c & 0x3F);
        } else if((c & 0x08) == 0) { // 4-byte UTF-8
            code_point = ((c & 0x7) << 18) | ((c & 0x3F) << 12) | ((c & 0x3F) << 6) | (c & 0x3F);
        } else {
            code_point = 0;
        }
    } else {
        code_point = c; // ASCII char
    }

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

    if(!chr) return; // Character not found, return early.

    // Uncompress and display the fragments
    ptr = chr + 6;
    o = (unsigned long)fb_info.pointer + y * fb_info.pitch + x * 4; // Calculate the pixel offset
    for(i = n = 0; i < chr[1]; i++, ptr += chr[0] & 0x40 ? 6 : 5) {
        if(ptr[0] == 255 && ptr[1] == 255) continue; // Skip empty fragments
        frg = (unsigned char*)font + (chr[0] & 0x40 ? 
            ((ptr[5] << 24) | (ptr[4] << 16) | (ptr[3] << 8) | ptr[2]) :
            ((ptr[4] << 16) | (ptr[3] << 8) | ptr[2]));
        
        if((frg[0] & 0xE0) != 0x80) continue; // Skip invalid fragments

        o += (int)(ptr[1] - n) * fb_info.pitch; // Update offset for next fragment
        n = ptr[1];

        k = ((frg[0] & 0x1F) + 1) << 3; // Character width
        j = frg[1] + 1; // Character height
        frg += 2; // Move to the next fragment data
        
        // Loop through the fragment data to render pixels
        for(m = 1; j; j--, n++, o += fb_info.pitch) {
            for(p = o, l = 0; l < k; l++, p += 4, m <<= 1) {
                if(m > 0x80) { frg++; m = 1; } // Move to the next byte if necessary
                if(*frg & m) {
                    *((unsigned int*)p) = attribute; // Set the pixel color (attribute)
                }
            }
        }
    }
}
