#include "uart.h"
#include "fb.h"

void main() {
    uart_init();
    fb_init();
    uart_puts("Welcome to RaspiOS!\n");

    uint32_t attr = 0xFFFFFF;

    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < 768; j++) {
            put_pixel(i, j, attr);
        }
    }
    while (1) {
        uart_putc(uart_getc());  // Echo UART input
    }
}