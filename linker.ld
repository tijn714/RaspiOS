ENTRY(_start)

SECTIONS
{
    . = 0x80000;

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
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        __bss_end = .;
    }
}
