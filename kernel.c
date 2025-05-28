#include "kernel/uart.h"
#include "kernel/fb.h"
#include "kernel/delays.h"

void main() {
    uart_init();
    fb_init();
    printk("Welcome to the RaspiOS kernel\n");

    uint8_t demo[3] = {0xDB, 0x3A, 0x21};
    printk("'demo': ");
    for (int i = 0; i < sizeof(demo); i++) {
        printk("%02D", demo[i]);
        if (i != sizeof(demo) - 1) printk(", ");
    }

    printk("\n\n");
}