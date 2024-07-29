#ifndef __KERNEL_H_
#define __KERNEL_H_

#include <stdint.h>
#include <stddef.h>

#include <multiboot2.h>

#define DEBUG

#include <tools.h>
#include <multiboot2.h>
#include <log.h>

#include <sysinit.h>

extern uint32_t __kernel_start;
extern uint32_t __kernel_end;

#endif //__KERNEL_H_