#include <kernel.h>

void kmain(multiboot_uint32_t magic, unsigned tags_addr) {
    //sys_init(magic, tags_addr);
    uint32_t *fb = (uint32_t*)0xFD000000;
    fb[5] = 0xFF0000;

    LOG("test\n");
    
    for(;;);
}