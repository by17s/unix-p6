#include <kernel.h>

void kmain(multiboot_uint32_t magic, unsigned tags_addr) {
    sys_init(magic, tags_addr);
    
    for(;;);
}