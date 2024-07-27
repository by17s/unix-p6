#include <kernel.h>

#include <drivers/vbe.h>

int kmain(multiboot_uint32_t magic, uint32_t addr)
{
  sys_init(magic, addr, 0);
  
  return 1;
}
