#include <stdint.h>    // Include standard integer types
#include "x86.h"      // Include x86 architecture-specific definitions

// Define I/O base addresses for master and slave PICs
#define PIC1            0x20    // IO base address for master PIC
#define PIC2            0xA0    // IO base address for slave PIC
#define PIC1_COMMAND    PIC1
#define PIC1_DATA       (PIC1 + 1)
#define PIC2_COMMAND    PIC2
#define PIC2_DATA       (PIC2 + 1)

// Initialization Command Word (ICW) definitions for PIC configuration
#define ICW1_ICW4      0x01    // Indicates that ICW4 will be present
#define ICW1_SINGLE    0x02    // Single (cascade) mode
#define ICW1_INTERVAL4 0x04    // Call address interval 4 (8)
#define ICW1_LEVEL     0x08    // Level triggered (edge) mode
#define ICW1_INIT      0x10    // Initialization - required!

// ICW4 definitions for operational mode settings
#define ICW4_8086      0x01    // 8086/88 mode
#define ICW4_AUTO      0x02    // Auto (normal) EOI
#define ICW4_BUF_SLAVE 0x08    // Buffered mode/slave
#define ICW4_BUF_MASTER 0x0C   // Buffered mode/master
#define ICW4_SFNM      0x10    // Special fully nested (not)

// End-of-Interrupt command code
#define PIC_EOI        0x20

// Send End-of-Interrupt signal to the appropriate PIC
void irq_sendeoi(uint8_t irq)
{
	if(irq >= 8) 
		outb(PIC2_COMMAND, PIC_EOI); // Send EOI to slave PIC if IRQ is from it

	outb(PIC1_COMMAND, PIC_EOI); // Always send EOI to master PIC
}

// Remap the IRQs to a new interrupt vector range starting from start_int
void irq_remap(uint8_t start_int)
{
	uint8_t a1 = inb(PIC1_DATA); // Save current masks for both PICs
	uint8_t a2 = inb(PIC2_DATA);

	// Start initialization sequence for both PICs in cascade mode
	outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();                    // Wait for the command to be processed
	outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();

	outb(PIC1_DATA, start_int);   // Set master PIC vector offset
	io_wait();
	outb(PIC2_DATA, start_int + 8); // Set slave PIC vector offset (offset by 8)
	io_wait();

	outb(PIC1_DATA, 4);           // Inform master that there is a slave at IRQ2
	io_wait();
	outb(PIC2_DATA, 2);           // Inform slave of its cascade identity
	io_wait();

	outb(PIC1_DATA, ICW4_8086);   // Set both PICs to operate in 8086 mode
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();

	outb(PIC1_DATA, a1);          // Restore saved masks for master PIC
	outb(PIC2_DATA, a2);          // Restore saved masks for slave PIC
}

// Set the mask for a specific IRQ line to disable it
void irq_setmask(uint8_t IRQline)
{
	uint16_t port;
	uint8_t value;

	if(IRQline < 8) {
		port = PIC1_DATA;          // Use master PIC data port for IRQs 0-7
	} else {
		port = PIC2_DATA;          // Use slave PIC data port for IRQs 8-15
		IRQline -= 8;              // Adjust IRQline index for slave port
	}
	value = inb(port) | (1 << IRQline);   // Set the appropriate bit to disable the IRQ line
	outb(port, value);        // Write back the updated mask to the port        
}

// Clear the mask for a specific IRQ line to enable it
void irq_clearmask(uint8_t IRQline)
{
	uint16_t port;
	uint8_t value;

	if(IRQline < 8) {
		port = PIC1_DATA;          // Use master PIC data port for IRQs 0-7
	} else {
		port = PIC2_DATA;          // Use slave PIC data port for IRQs 8-15
		IRQline -= 8;              // Adjust IRQline index for slave port
	}
	value = inb(port) & ~(1 << IRQline);   // Clear the appropriate bit to enable the IRQ line
	outb(port, value);        // Write back the updated mask to the port        
}

// Clear all masks on both PICs to enable all interrupts 
void irq_clear(void)
{
	outb(PIC1_DATA, 0xff);     // Disable all interrupts on master PIC
	outb(PIC2_DATA, 0xff);     // Disable all interrupts on slave PIC 
}
