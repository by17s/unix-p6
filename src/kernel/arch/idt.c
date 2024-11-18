c
#include <stdio.h>        // Include standard input/output functions
#include <stdint.h>      // Include standard integer types
#include "x86.h"        // Include x86 architecture-specific definitions
#include <log.h>        // Include logging functions

// Define IDT entry and IRQ handler tables
idt_entry_t idt_table[256];          // IDT with 256 entries
irq_handler irq_handlertable[64];    // Table for IRQ handlers
idtr_t idtr;                          // IDTR structure for IDT base and limit

// External references to ISR and IRQ tables
extern void* isr_table[];
extern void* irq_table[];

// Array of exception names for logging

const char* exception_names[] = {
    "DivideByZero",             //0
    "Debug",                    //1
    "NMI",                      //2
    "Breakpoint",               //3
    "Overflow",                 //4
    "BoundRangeExceed",         //5
    "InvalidOpcode",            //6
    "CoprocessorNotAviable",    //7
    "DoubleFault",              //8
    "CoprocessorSegmentOverrun",//9
    "InvalidTSS",               //10
    "SegmentNotPresent",        //11
    "StackFault",               //12
    "GeneralProtection",        //13
    "PageFault",                //14
    "",                         //15
    "x87",                      //16
    "AlignmentCheck",           //17
    "MachineCheck",             //18
    "SIMD",                     //19
    "Virtualization",           //20
    "ControlProtection",        //21
    "",                         //22
    "",                         //23
    "",                         //24
    "",                         //25
    "",                         //26
    "",                         //27
    "",                         //28
    "HypervisorInjection",      //29
    "VMMCommunication",         //30
    "SecurityException"         //31
};

// Set an entry in the IDT
void idt_setentry(uint8_t vector, void* isr, uint16_t cs, uint8_t flags)
{
    idt_table[vector].isr_low = (uint16_t)((uintptr_t)isr);  // Set low part of ISR address
    idt_table[vector].isr_high = (uint16_t)((uintptr_t)isr >> 16);  // Set high part of ISR address
    idt_table[vector].gdt_cs = cs;  // Set code segment selector
    idt_table[vector].res = 0;       // Reserved field
    idt_table[vector].flags = flags; // Set flags (type and attributes)
}

// Set an IRQ handler for a specific line in the IRQ table
void idt_setirqhandler(uint8_t line, irq_handler handler, uint8_t enable)
{
    if(line > 15) return;  // Check if line is valid (0-15)

    irq_handlertable[line] = handler;  // Assign the handler

    if(enable)
        irq_clearmask(line);  // Clear the mask if enabling the handler
}

// Initialize the IDT and related structures
void idt_init() {
    idtr.offset = (uintptr_t)&idt_table;  // Set IDTR offset to IDT base address
    idtr.limit = sizeof(idt_table) - 1;   // Set limit to size of IDT

    tmemset(&idt_table, 0, sizeof(idt_table));  // Zero out the IDT table

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_setentry(vector, isr_table[vector], 0x08, 0x8E); // Set exception handlers in IDT
    }

    irq_remap(0x20);   // Remap IRQs to avoid conflicts with exceptions
    irq_clear();       // Clear any pending IRQs

    for (uint8_t i = 0; i < 16; i++) {
        idt_setentry(i + 0x20, irq_table[i], 0x08, 0x8E); // Set IRQ handlers in IDT
    }

    lidt(&idtr);       // Load IDTR with the new IDT base and limit
    sti();            // Enable interrupts globally
}

// Handle exceptions by logging details and halting the system
void handle_exception(isrctx_t ctx)
{
    LOG("\nAn exception occurred!\nISR:%i, Name:%s", ctx.intnum, exception_names[ctx.intnum]); 
    if(ctx.errcode)
        LOG(", Error Code:%x", ctx.errcode >> 3); // Log error code if available
    LOG(".\n");

    cli();           // Disable interrupts while handling exception
    while(1)
        hlt();       // Halt the CPU in an infinite loop until reset
}

// Handle IRQ interrupts by calling the appropriate handler and sending end-of-interrupt signal
void handle_irqint(irqctx_t ctx)
{
    if(irq_handlertable[ctx.irq])  
        irq_handlertable[ctx.irq](&ctx); // Call the registered IRQ handler

    irq_sendeoi(ctx.irq);   // Send end-of-interrupt signal to the PIC/IOAPIC
}