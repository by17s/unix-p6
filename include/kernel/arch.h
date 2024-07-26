#ifndef __ARCH_H_
#define __ARCH_H_

#include <tools.h>

// == arch/ports.c ==
unsigned char inb(unsigned short port);

void outb(unsigned short port, unsigned char data);

uint16_t inw(uint16_t port);

void outw(unsigned short port, unsigned short data);

// == arch/com.c ==
#define COM1_PORT 0x3f8

void com_write(char data);

#endif //__ARCH_H_