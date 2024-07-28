#ifndef __SYSINIT_H_
#define __SYSINIT_H_

#include <multiboot2.h>

#include <tools.h>

typedef struct {
    
} sys_config_t;

int sys_init(multiboot_uint32_t magic, uint32_t addr, int flags);

#endif //__SYSINIT_H_