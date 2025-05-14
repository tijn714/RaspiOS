#ifndef UART_H 
#define UART_H

#include "gpio.h"

void uart_init();
void uart_send(unsigned int c);
char uart_getc();
void uart_putc(char c);
void uart_puts(char *s);
void uart_hex(unsigned int d);
void uart_hex64(uint64_t val);
void uart_dec(uint32_t val);

extern uint64_t dtb_ptr;

#endif // UART_H