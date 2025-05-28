#include "kernel/gpio.h"

/* mailbox message buffer */
volatile unsigned int  __attribute__((aligned(16))) mbox[36];

#define VIDEOCORE_MBOX  (PERIPHERAL_BASE+0x0000B880)
#define MBOX_READ       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x0))
#define MBOX_POLL       ((volatile unsigned int*)(VIDEOCORE_MBOX+0x10))
#define MBOX_SENDER     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x14))
#define MBOX_STATUS     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x18))
#define MBOX_CONFIG     ((volatile unsigned int*)(VIDEOCORE_MBOX+0x1C))
#define MBOX_WRITE      ((volatile unsigned int*)(VIDEOCORE_MBOX+0x20))
#define MBOX_RESPONSE   0x80000000
#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

int mbox_call(unsigned char ch)
{
    unsigned int r = (((unsigned int)((unsigned long)&mbox)&~0xF) | (ch&0xF));
    /* wait until we can write to the mailbox */
    do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_FULL);
    /* write the address of our message to the mailbox with channel identifier */
    *MBOX_WRITE = r;
    /* now wait for the response */
    while(1) {
        /* is there a response? */
        do{asm volatile("nop");}while(*MBOX_STATUS & MBOX_EMPTY);
        /* is it a response to our message? */
        if(r == *MBOX_READ)
            /* is it a valid successful response? */
            return mbox[1]==MBOX_RESPONSE;
    }
    return 0;
}

int get_arm_memory_size() {
    mbox[0] = 8 * 4;                // size of the message buffer (in bytes)
    mbox[1] = 0x00000000;           // request code
    mbox[2] = 0x00010005;           // tag identifier (GET_ARM_MEMORY)
    mbox[3] = 8;                    // value buffer size
    mbox[4] = 0;                    // request size
    mbox[5] = 0;                    // response: base address (filled by VC)
    mbox[6] = 0;                    // response: size (filled by VC)
    mbox[7] = 0;                    // end tag

    if (mbox_call(8)) {            // Channel 8 is the property channel
        // RAM size is in mbox[6]
        return mbox[6];
    } else {
        return -1; // failed
    }
}
