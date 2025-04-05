#ifndef UART_H 
#define UART_H

#include "gpio.h"

void uart_init();
void uart_putc(char c);
char uart_getc();
void uart_puts(const char *str);

#endif // UART_H