ENTRY(_start)

SECTIONS
{
    . = 0x80000;  /* Typical kernel load address for RPi 4 */
    
    .text : {
        *(.text.start)  /* Startup code */
        *(.text*)       /* Rest of the code */
    }
    
    .rodata : {
        *(.rodata*)
    }
    
    .data : {
        *(.data*)
    }
    
    .bss : {
        *(COMMON)
        *(.bss*)
    }
    
    /* Define symbols for BSS section */
    __bss_start = .;
    __bss_end = .;
}