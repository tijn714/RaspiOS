#include "uart.h"

void main() {
    uart_init();
    uart_puts("Welcome to RaspiOS!\n");

    while (1) {
        uart_putc(uart_getc());  // Echo UART input
    }
}