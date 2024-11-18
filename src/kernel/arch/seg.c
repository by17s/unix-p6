#include "x86.h"  // Include x86 architecture-specific definitions

gdt_entry_t gdt_table[NUMSEGS];  // Array to hold GDT entries
gdtr_t gdtr;                      // GDTR structure to hold GDT base and limit

// Initialize the Global Descriptor Table (GDT)
void seg_init()
{
    gdtr.offset = (uintptr_t)&gdt_table;  // Set GDTR offset to the GDT base address
    gdtr.limit = sizeof(gdt_table) - 1;   // Set GDTR limit to the size of the GDT

    tmemset(&gdt_table[0], 0, sizeof(gdt_table[0])); // Zero out the first GDT entry (null descriptor)

    // Set up segment descriptors in the GDT
    gdt_setgate(&gdt_table[1], 0x0, 0xFFFFF, DPL_KRNL, 0, IS_CS); // Kernel code segment
    gdt_setgate(&gdt_table[2], 0x0, 0xFFFFF, DPL_KRNL, 0, IS_DS); // Kernel data segment
    gdt_setgate(&gdt_table[3], 0x0, 0xFFFFF, DPL_USER, 0, IS_CS); // User code segment
    gdt_setgate(&gdt_table[4], 0x0, 0xFFFFF, DPL_USER, 0, IS_DS); // User data segment

    gdt_flush(&gdtr); // Load the new GDT into the processor
}
