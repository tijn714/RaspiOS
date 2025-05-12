#include <stdint.h>
#include "uart.h"
#include "dtb.h"

static inline uint32_t bswap(uint32_t x) {
    return __builtin_bswap32(x);
}

static void parse_dtb(uint8_t* dtb) {
    fdt_header_t* hdr = (fdt_header_t*)dtb;

    if (bswap(hdr->magic) != FDT_MAGIC) {
        uart_puts("Invalid DTB0 magic!\n");
        return;
    }

    uint32_t struct_off = bswap(hdr->off_dt_struct);
    uint32_t strings_off = bswap(hdr->off_dt_strings);
    uint8_t* struct_ptr = dtb + struct_off;
    uint8_t* strings_ptr = dtb + strings_off;

    int depth = 0;
    while (1) {
        uint32_t token = bswap(*(uint32_t*)struct_ptr);
        struct_ptr += 4;

        if (token == FDT_BEGIN_NODE) {
            const char* name = (const char*)struct_ptr;
            uart_puts("Node: ");
            uart_puts(name);
            uart_puts("\n");

            struct_ptr += strlen(name) + 1;
            struct_ptr = (uint8_t*)(((uintptr_t)struct_ptr + 3) & ~3); // align

            depth++;

        } else if (token == FDT_END_NODE) {
            depth--;
        } else if (token == FDT_PROP) {
            fdt_prop_t* prop = (fdt_prop_t*)struct_ptr;
            uint32_t len = bswap(prop->len);
            uint32_t nameoff = bswap(prop->nameoff);
            struct_ptr += sizeof(fdt_prop_t);

            const char* name = (const char*)(strings_ptr + nameoff);
            uint8_t* value = struct_ptr;

            if (!strcmp(name, "reg") && depth == 1) {
                // Memory size is second 32-bit word in reg
                uint64_t base = ((uint64_t)bswap(*(uint32_t*)&value[0]) << 32) | bswap(*(uint32_t*)&value[4]);
                uint64_t size = ((uint64_t)bswap(*(uint32_t*)&value[8]) << 32) | bswap(*(uint32_t*)&value[12]);

                uart_puts("RAM base: 0x");
                uart_hex64(base);
                uart_puts("\nRAM size: 0x");
                uart_hex64(size);
                uart_puts("\n");
            }

            if (!strcmp(name, "clock-frequency")) {
                uint32_t freq = bswap(*(uint32_t*)value);
                uart_puts("CPU Clock: ");
                uart_dec(freq / 1000000);
                uart_puts(" MHz\n");
            }

            struct_ptr += (len + 3) & ~3; // Align
        } else if (token == FDT_NOP) {
            continue;
        } else if (token == FDT_END) {
            break;
        } else {
            uart_puts("Unknown FDT token\n");
            break;
        }
    }
}
