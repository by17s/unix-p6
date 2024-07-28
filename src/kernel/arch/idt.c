#include <x86.h>

#include <log.h>

#define DIVISION_BY_ZERO 0x0
#define OVERFLOW 0x4
#define DOUBLE_FAULT 0x8
#define SEGMENTATION_FAULT 0x0C
#define PROTECTION_FAULT 0x0D
#define INVALID_OPCODE 0x6
#define PAGE_FAULT 0x0E

#define SYSCALL 0x80
#define SYSCALL_SPECIAL 0x50

#define IDT_TRAP_GATE         0x8F
#define IDT_USER_GATE         0xEE

typedef struct {
    uint16_t    offsetLow;   // Lower 16bits of isr address
    uint16_t  codeSegment;   // Code segment for this ISR
    uint8_t          zero;   // Set to 0, reserved by intel
    uint8_t         flags;   // Type and attributes; Flags
    uint16_t   offsetHigh;   // Upper 16bits of isr address
} __attribute__ ((packed)) IDT_ENTRY_t;

typedef struct {
    uint16_t  limit;
    uint32_t   base;
} __attribute__ ((packed)) IDR_RECORD_t;

IDT_ENTRY_t idt[256];
IDR_RECORD_t idtRecord;

void idt_set_PIC() {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);
}

__attribute__((interrupt)) void idt_handler_stub(void *frame) {
    return;
}

void idt_set_gate(int int_num, void* isr, uint8_t flags) {
    IDT_ENTRY_t *idtEntry = &idt[int_num];

    idtEntry->offsetLow = (uintptr_t)isr & 0xFFFF;
    idtEntry->codeSegment = 0x08;
    idtEntry->zero = 0;
    idtEntry->flags = flags;
    idtEntry->offsetHigh = ((uintptr_t)isr >> 16) & 0xFFFF;
}

void idt_set_basic_handlers() {
//    Exceptions && Errors
    /*
    idt_set_gate(DIVISION_BY_ZERO,  (void*)exception_handler_divisionByZero,    IDT_TRAP_GATE);
    idt_set_gate(OVERFLOW,          (void*)exception_handler_overflow,          IDT_TRAP_GATE);
    idt_set_gate(DOUBLE_FAULT,      (void*)exception_handler_doubleFault,       IDT_TRAP_GATE);
    idt_set_gate(SEGMENTATION_FAULT,(void*)exception_handler_segmentationFault, IDT_TRAP_GATE);
    idt_set_gate(PROTECTION_FAULT,  (void*)exception_handler_protectionFault,   IDT_TRAP_GATE);
    idt_set_gate(PAGE_FAULT,        (void*)exception_handler_pageFault,         IDT_TRAP_GATE);
    */
    //idt_set_gate(SYSCALL,           (void*)0,                            IDT_USER_GATE);
}

void idt_set_stub_handlers() {
    for (int i = 0; i < 256; ++i) {
        if (idt[i].flags == 0) {
            idt_set_gate(i, (void*)idt_handler_stub, IDT_TRAP_GATE);
        }
    }
}

void idt_install(void) {
    LOG("Installing IDT...\n");
    idtRecord.limit = (uint16_t)(sizeof idt);
    idtRecord.base = (uintptr_t)&idt;

    idt_set_basic_handlers();
    idt_set_stub_handlers();
    idt_set_PIC();

    asm volatile("lidt %0" : : "memory"(idtRecord));
    __asm__ volatile("sti");

    LOG("IDT has been successfully installed and is ready to use.\n");
    return;
}