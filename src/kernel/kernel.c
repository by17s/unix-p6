#include <kernel.h>
#include <drivers/vbe.h>
#include <x86.h>

int kmain(multiboot_uint32_t magic, uint32_t addr)
{
  seg_init();
  sys_init(magic, addr, 0);  
  int w, h;
  vbe_sendcmd(VBE_GET_RES, &w, &h);
  LOG("res %dx%d\n", w, h);
  
  return 1;
}
