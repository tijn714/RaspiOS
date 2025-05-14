.section ".text.start"

.global _start
_start:
    // Save DTB address from x0
    mov    x20, x0         // Save DTB pointer in x20

    // Store DTB pointer to global variable
    ldr    x1, =dtb_ptr
    str    x20, [x1]

    // Get CPU ID from MPIDR_EL1
    mrs    x1, mpidr_el1
    and    x1, x1, #0x3    // Mask to get CPU ID (0-3)
    cbz    x1, core0_run   // If core 0, continue
    b      halt            // Other cores halt

halt:
    wfe                    // Wait for event
    b      halt

.global fetch_dtb0
fetch_dtb0:
    mov x0, x20
    ret

core0_run:
    mov    sp, #0x80000

    // Clear BSS section
    ldr    x1, =__bss_start
    ldr    x2, =__bss_end
bss_loop:
    cmp    x1, x2
    bge    bss_done
    str    xzr, [x1], #8
    b      bss_loop
bss_done:

    ldr    x1, =dtb_ptr
    str    x20, [x1]

    ldr    x1, =0x44544230
    str    w1, [x20]

    bl     main
    b      halt

// Reserve space for DTB pointer (C-accessible)
.section .bss
.align 8
.global dtb_ptr
dtb_ptr:
    .skip 8   // Reserve 8 bytes for uint64_t DTB pointer
