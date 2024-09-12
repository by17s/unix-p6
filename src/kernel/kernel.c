#include <kernel.h>
#include <drivers/vbe.h>
#include <x86.h>
#include <modules.h>
#include <fs/tar.h>

int kmain(multiboot_uint32_t magic, uint32_t addr)
{
  seg_init();
  idt_init();
  sys_init(magic, addr, 0);  

  /*int w, h;
  vbe_sendcmd(VBE_GET_RES, &w, &h);
  LOG("res %dx%d\n", w, h);*/

  module_t *mdl = module_get(0);
  LOG("paddr=%x size=%u\n", mdl->mod_start, mdl->mod_end - mdl->mod_start);
  tar_t *tar = tar_init((char*)mdl->mod_start, mdl->mod_end - mdl->mod_start);
  tar_t file = tar_get_file(tar, "initrd/tar.h");
  LOG("%s", file.ptr);
  
  return 1;
}
