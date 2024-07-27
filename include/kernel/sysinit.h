#ifndef __ARCH_H_
#define __ARCH_H_

#include <multiboot2.h>

#include <tools.h>

typedef struct {
    
} sys_config_t;

int sys_init(multiboot_uint32_t magic, uint32_t addr, int flags);

#endif //__ARCH_H_