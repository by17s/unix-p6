#include <kernel.h>
#include <drivers/vbe.h>
#include <x86.h>
#include <modules.h>
#include <vfs.h>
#include <mem.h>

// Kernel main entry point
int kmain(multiboot_uint32_t magic, uint32_t addr)
{
    seg_init();           // Initialize segment descriptors
    idt_init();           // Initialize the Interrupt Descriptor Table (IDT)
    sys_init(magic, addr, 0); // Initialize system with multiboot information

    vfs_init();
    vfs_mount("/dev/null", (void*)1); //test
    /* 
    Uncommented section to get VBE resolution:
    int w, h;
    vbe_sendcmd(VBE_GET_RES, &w, &h);
    LOG("res %dx%d\n", w, h);
    */

    module_t *mdl = module_get(0); // Retrieve the first loaded module
    LOG("paddr=%x size=%u\n", mdl->mod_start, mdl->mod_end - mdl->mod_start); // Log module start address and size
    
    // tar_t *tar = tar_init((char*)mdl->mod_start, mdl->mod_end - mdl->mod_start); // Initialize TAR from module data
    // tar_t file = tar_get_file(tar, "initrd/tar.h"); // Retrieve specific file from TAR archive
    // LOG("%s", file.ptr); // Log the contents of the retrieved file

    return 1; // Return success code (or some status)
}
