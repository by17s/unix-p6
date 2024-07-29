#include <stdio.h>
#include <stdint.h>
#include "x86.h"
#include <log.h>

idt_entry_t idt_table[256];

irq_handler irq_handlertable[64];

idtr_t idtr;

extern void* isr_table[];
extern void* irq_table[];

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

void idt_setentry(uint8_t vector, void* isr, uint16_t cs, uint8_t flags)
{
    idt_table[vector].isr_low = (uint16_t)((uintptr_t)isr);
    idt_table[vector].isr_high = (uint16_t)((uintptr_t)isr >> 16);
    idt_table[vector].gdt_cs = cs;
    idt_table[vector].res = 0;
    idt_table[vector].flags = flags;
}

void idt_setirqhandler(uint8_t line, irq_handler handler, uint8_t enable)
{
    if(line > 15)
        return;

    irq_handlertable[line] = handler;

    if(enable)
        irq_clearmask(line);
}

void idt_init() {
    idtr.offset = (uintptr_t)&idt_table;
    idtr.limit = sizeof(idt_table) - 1;

    tmemset(&idt_table, 0, sizeof(idt_table));

    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_setentry(vector, isr_table[vector], 0x08, 0x8E);
    }

    irq_remap(0x20);
    irq_clear();

    for (uint8_t i = 0; i < 16; i++) {
        idt_setentry(i + 0x20, irq_table[i], 0x08, 0x8E);
    }

    lidt(&idtr);
    sti();
}

void handle_exception(isrctx_t ctx)
{
    LOG("\nAn exception occurred!\nISR:%i, Name:%s", ctx.intnum, exception_names[ctx.intnum]);
    if(ctx.errcode)
        LOG(", Error Code:%x", ctx.errcode >> 3);
    LOG(".\n");

    cli();
    while(1)
        hlt();
}

void handle_irqint(irqctx_t ctx)
{
    if(irq_handlertable[ctx.irq])
        irq_handlertable[ctx.irq](&ctx);

    irq_sendeoi(ctx.irq);
}