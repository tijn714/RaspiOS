#include "uart.h"
#include "fb.h"

void main() {
    uart_init();
    fb_init();
    uart_puts("Welcome to RaspiOS!\n");

    putchar(10, 10, "H", 0xFFFFFF);
    putchar(18, 10, "o", 0xFFFFFF);
    putchar(26, 10, "i", 0xFFFFFF);
    putchar(34, 10, "!", 0xFFFFFF);

    putchar(10, 30, "П", 0xFFFFFF);
    putchar(18, 30, "р", 0xFFFFFF);
    putchar(26, 30, "и", 0xFFFFFF);
    putchar(34, 30, "в", 0xFFFFFF);
    putchar(42, 30, "е", 0xFFFFFF);
    putchar(50, 30, "т", 0xFFFFFF);
    putchar(58, 30, "!", 0xFFFFFF);

    while (1) {
        uart_putc(uart_getc());  // Echo UART input
    }
}