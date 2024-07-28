#include <sysinit.h>

#include <multiboot2.h>
#include <log.h>

#include <mem.h>

#include <drivers/tvga.h>
#include <drivers/vbe.h>

const char *__mb2_htags_str[] = {
    "TAG_TYPE_END               ",
    "TAG_TYPE_CMDLINE           ",
    "TAG_TYPE_BOOT_LOADER_NAME  ",
    "TAG_TYPE_MODULE            ",
    "TAG_TYPE_BASIC_MEMINFO     ",
    "TAG_TYPE_BOOTDEV           ",
    "TAG_TYPE_MMAP              ",
    "TAG_TYPE_VBE               ",
    "TAG_TYPE_FRAMEBUFFER       ",
    "TAG_TYPE_ELF_SECTIONS      ",
    "TAG_TYPE_APM               ",
    "TAG_TYPE_EFI32             ",
    "TAG_TYPE_EFI64             ",
    "TAG_TYPE_SMBIOS            ",
    "TAG_TYPE_ACPI_OLD          ",
    "TAG_TYPE_ACPI_NEW          ",
    "TAG_TYPE_NETWORK           ",
    "TAG_TYPE_EFI_MMAP          ",
    "TAG_TYPE_EFI_BS            ",
    "TAG_TYPE_EFI32_IH          ",
    "TAG_TYPE_EFI64_IH          ",
    "TAG_TYPE_LOAD_BASE_ADDR    ",
};

const char *__mb2_htag_fb_str[] = {
    "FRAMEBUFFER_TYPE_INDEXED",
    "FRAMEBUFFER_TYPE_RGB",
    "FRAMEBUFFER_TYPE_EGA_TEXT",
};

int sys_init(multiboot_uint32_t magic, uint32_t addr, int flags) {
    struct multiboot_tag *tag;
  unsigned size;

  /*  Am I booted by a Multiboot-compliant boot loader? */
  if (magic != MULTIBOOT2_BOOTLOADER_MAGIC)
  {
    LOG(" -> Invalid magic number: 0x%x\n", (unsigned)magic);
    return -1;
  }

  if (addr & 7)
  {
    LOG(" -> Unaligned mbi: 0x%x\n", addr);
    return -1;
  }

  LOG("[INF ] Reading multiboot data...\n");

  size = *(unsigned *)addr;
  LOG(" MBI located at address 0x%x\n", addr);
  for (tag = (struct multiboot_tag *)(addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
  {
    LOG("Tag: %s, Size: %u Byte\n", __mb2_htags_str[tag->type], tag->size);
    switch (tag->type)
    {
    case MULTIBOOT_TAG_TYPE_CMDLINE:
      LOG(" -> CMD_LINE: = %s\n",
               ((struct multiboot_tag_string *)tag)->string);
      break;
    case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
      LOG(" -> Boot loader name = %s\n",
               ((struct multiboot_tag_string *)tag)->string);
      break;
    case MULTIBOOT_TAG_TYPE_MODULE:
      LOG(" -> Module located from 0x%x to 0x%x. CMD_LINE: %s\n",
               ((struct multiboot_tag_module *)tag)->mod_start,
               ((struct multiboot_tag_module *)tag)->mod_end,
               ((struct multiboot_tag_module *)tag)->cmdline);
      break;
    case MULTIBOOT_TAG_TYPE_BASIC_MEMINFO:
      LOG(" -> mem_lower = %uKB, mem_upper = %uKB\n",
               ((struct multiboot_tag_basic_meminfo *)tag)->mem_lower,
               ((struct multiboot_tag_basic_meminfo *)tag)->mem_upper);
      break;
    case MULTIBOOT_TAG_TYPE_BOOTDEV:
      LOG(" -> Boot device: 0x%x,%u,%u\n",
               ((struct multiboot_tag_bootdev *)tag)->biosdev,
               ((struct multiboot_tag_bootdev *)tag)->slice,
               ((struct multiboot_tag_bootdev *)tag)->part);
      break;
    case MULTIBOOT_TAG_TYPE_MMAP:
    {
      multiboot_memory_map_t *mmap;

      LOG(" -> Memory map\n");
      size_t free_sz;
      void* free_ptr;
      for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
           (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
           mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size)) {
            LOG(" --> Address: 0x%x%x,"
                 " Size: 0x%x%x, Type: 0x%x\n",
                 (unsigned)(mmap->addr >> 32),
                 (unsigned)(mmap->addr & 0xffffffff),
                 (unsigned)(mmap->len >> 32),
                 (unsigned)(mmap->len & 0xffffffff),
                 (unsigned)mmap->type);
            if(mmap->addr > 0 && mmap->addr < UINT32_MAX && mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
              free_ptr = (void*)(unsigned)(mmap->addr >> 32);
              free_sz = mmap->len;
            }
           }
           mm_init(free_ptr, free_sz);
        
    }
      break;
    case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
    {
      multiboot_uint32_t color;
      unsigned i;
      struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;
      void *fb = (void *)(unsigned long)tagfb->common.framebuffer_addr;
      LOG(" -> Framebuffer type: %s\n",
               __mb2_htag_fb_str[tagfb->common.framebuffer_type]);
      LOG(" -> Resolution: %ux%u px, BPP: %u bits\n",
               tagfb->common.framebuffer_width,
               tagfb->common.framebuffer_height,
               tagfb->common.framebuffer_bpp);
      LOG(" -> Address: 0x%x\n", tagfb->common.framebuffer_addr);
      switch (tagfb->common.framebuffer_type)
      {
      case MULTIBOOT_FRAMEBUFFER_TYPE_EGA_TEXT:
        tvga_init();
        break;
      case MULTIBOOT_FRAMEBUFFER_TYPE_RGB:
        vbe_init(
            (uintptr_t*)(uintptr_t)tagfb->common.framebuffer_addr,
            tagfb->common.framebuffer_width,
            tagfb->common.framebuffer_height,
            tagfb->common.framebuffer_bpp,
            vbe_color_palette_RGB
        );
        break;
      default:
        break;
      }
      break;
    }
    }
  }
  tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7));
  LOG(" MBI size: %u bytes\n", (unsigned)tag - addr);
  LOG("[INF ] Bootloader data reading complete.\n");
    return 0;
}