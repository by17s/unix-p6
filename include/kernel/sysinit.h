#ifndef __SYSINIT_H_
#define __SYSINIT_H_

#include <tools.h>
#include <multiboot2.h>
#include <log.h>

struct sys_info
{
	struct multiboot_tag_framebuffer framebuf;
};

int sys_init(multiboot_uint32_t magic, size_t addr, struct sys_info *info);

#endif //__SYSINIT_H_