#include "gpio.h"
#include "uart.h"

void uart_init() {
    // Disable UART before configuration
    REG(UART_CR) = 0;

    // Set baud rate (115200 @ 48MHz clock)
    REG(UART_IBRD) = 26;    // Integer part
    REG(UART_FBRD) = 3;     // Fractional part

    // Set 8 bits, no parity, 1 stop bit, FIFOs enabled
    REG(UART_LCRH) = (3 << 5);  // 8 bits

    // Enable UART, TX, and RX
    REG(UART_CR) = (1 << 0) | (1 << 8) | (1 << 9);

    // Clear and disable all interrupts
    REG(UART_IMSC) = 0;
    REG(UART_ICR) = 0x7FF;
}

void uart_putc(char c) {
    // Wait until transmit FIFO has space
    while (REG(UART_FR) & FR_TXFF) {}
    REG(UART_DR) = c;
}

char uart_getc() {
    // Wait until receive FIFO has data
    while (REG(UART_FR) & FR_RXFE) {}
    return (char)REG(UART_DR);
}

void uart_puts(const char *str) {
    while (*str) {
        uart_putc(*str++);
    }
}