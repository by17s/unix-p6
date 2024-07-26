#ifndef __SYSINIT_H_
#define __SYSINIT_H_

#include <tools.h>
#include <multiboot2.h>
#include <log.h>

int sys_init(multiboot_uint32_t magic, size_t addr);

#endif //__SYSINIT_H_