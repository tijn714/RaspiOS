#include "uart.h"
#include "fb.h"

void main() {
    uart_init();
    fb_init();
    uart_puts("Welcome to RaspiOS!\n");


    for (int i = 0; i < 1024; i++) {
        for (int j = 0; j < 768; j++) {
            put_pixel(i, j, 0x0000FF); // Make the entire screen 
        }
    }

    putchar(10, 10, 'H', 0xFFFFFF);
    putchar(18, 10, 'e', 0xFFFFFF);
    putchar(26, 10, 'l', 0xFFFFFF);
    putchar(32, 10, 'l', 0xFFFFFF);
    putchar(40, 10, 'o', 0xFFFFFF);
    putchar(48, 10, '!', 0xFFFFFF);


    while (1) {
        uart_putc(uart_getc());  // Echo UART input
    }
}