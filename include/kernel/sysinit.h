#ifndef __SYSINIT_H_
#define __SYSINIT_H_

#include <multiboot2.h>

#include <tools.h>

enum DISPLAY_DRIVER_ {
    DISPLAY_DRIVER_TVGA = 2,
    DISPLAY_DRIVER_VBE = 1
};

typedef struct {
    enum DISPLAY_DRIVER_ disp_drv;
} sys_config_t;

#ifndef __IS_SYSINIT_C
    extern const sys_config_t sys_config;
#endif

int sys_init(multiboot_uint32_t magic, uint32_t addr, int flags);

#endif //__SYSINIT_H_