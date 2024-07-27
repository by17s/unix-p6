#ifndef __ARCH_H_
#define __ARCH_H_

#include <tools.h>

typedef struct {
    uint16_t limit_low : 16;        // Low bits of segment limit
    uint16_t offset_low : 16;       // Low bits of segment base address
    uint8_t offset_middle : 8;      // Middle bits of segment base address
    union
    {
        struct
        {
            uint8_t a : 1;
            uint8_t rw : 1;
            uint8_t dc : 1;
            uint8_t e : 1;
            uint8_t s : 1;          
            uint8_t dpl : 2;        // Descriptor Privilege Level
            uint8_t p : 1;          // Is Present
        };
        struct
        {
            uint8_t type : 4;
        };
        struct
        {
            uint8_t accbyte;
        };
    };
    uint8_t limit_high : 4;         // High bits of segment limit
    uint8_t res1 : 1;               // Reserved
    uint8_t res2 : 1;               // Reserved (In AMD64 its long mode flag)
    uint8_t db : 1;                 // 0 = 16-bit segment, 1 = 32-bit segment
    uint8_t g : 1;                  // Granularity: limit scaled by 4K when set
    uint8_t offset_high : 8;        // High bits of segment base address
} gdt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t offset;
} __attribute__((packed)) gdtr_t;

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ( "outb %b0, %w1" :: "a"(val), "Nd"(port) : "memory");
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ( "inb %w1, %b0" : "=a"(ret) : "Nd"(port) : "memory"); 
    return ret;
}

static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ("outw %0, %1" :: "a"(val), "Nd"(port));
}

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void lgdt(gdtr_t* gdtr)
{
    __asm__ volatile ("lgdt %0" :: "m"(*gdtr));
}

static inline uint32_t gcr0()
{
    uint32_t ret;
    __asm__ volatile("mov %%cr0, %0" : "=r"(ret));
    return ret;
}

static inline void lcr0(uint32_t cr0)
{
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

static inline void lcr3(uint32_t* pagedir)
{
    __asm__ volatile("mov %0, %%cr3" : : "r"(pagedir));
}

static inline void cli()
{
    __asm__ volatile ("cli");
}

static inline void sti()
{
    __asm__ volatile ("sti");
}

static inline void hlt()
{
    __asm__ volatile ("hlt");
}

static inline void io_wait(void)
{
    __asm__ volatile ( "outb %%al, $0x80" : : "a"(0) );
}

// == arch/com.c ==
#define COM1_PORT 0x3f8

void com_write(char data);

#endif //__ARCH_H_