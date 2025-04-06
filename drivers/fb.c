#include "uart.h"
#include "mbox.h"
#include "delays.h"

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

// Declare a global instance of the struct
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
