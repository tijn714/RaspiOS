#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <stdbool.h>

#define PERIPHERAL_BASE 0xFE000000
#define UART_BASE (PERIPHERAL_BASE + 0x201000)  // UART0 base address for RPi 4

// Register offsets
#define UART_DR   0x00    // Data Register
#define UART_FR   0x18    // Flag Register
#define UART_IBRD 0x24    // Integer Baud Rate Divisor
#define UART_FBRD 0x28    // Fractional Baud Rate Divisor
#define UART_LCRH 0x2C    // Line Control Register
#define UART_CR   0x30    // Control Register
#define UART_IMSC 0x38    // Interrupt Mask Set/Clear Register
#define UART_ICR  0x44    // Interrupt Clear Register

#define GPFSEL0         ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200000))
#define GPFSEL1         ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200004))
#define GPFSEL2         ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200008))
#define GPFSEL3         ((volatile unsigned int*)(PERIPHERAL_BASE+0x0020000C))
#define GPFSEL4         ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200010))
#define GPFSEL5         ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200014))
#define GPSET0          ((volatile unsigned int*)(PERIPHERAL_BASE+0x0020001C))
#define GPSET1          ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200020))
#define GPCLR0          ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200028))
#define GPLEV0          ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200034))
#define GPLEV1          ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200038))
#define GPEDS0          ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200040))
#define GPEDS1          ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200044))
#define GPHEN0          ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200064))
#define GPHEN1          ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200068))
#define GPPUD           ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200094))
#define GPPUDCLK0       ((volatile unsigned int*)(PERIPHERAL_BASE+0x00200098))
#define GPPUDCLK1       ((volatile unsigned int*)(PERIPHERAL_BASE+0x0020009C))

// Flag Register bits
#define FR_TXFF (1 << 5)  // Transmit FIFO full
#define FR_RXFE (1 << 4)  // Receive FIFO empty

// Volatile access to registers
#define REG(x) (*(volatile unsigned int *)(UART_BASE + (x)))


#endif // GPIO_H