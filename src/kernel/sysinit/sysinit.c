#include <kernel.h>

#include <sysinit.h>

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

struct multiboot_tag_mmap *_mmap_mb2 = NULL;

int sys_init(multiboot_uint32_t magic, size_t addr, struct sys_info *info)
{
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

  LOG("[INF ] Чтение данных загрузчика...\n");

  size = *(unsigned *)addr;
  LOG(" MBI расположен по адресу 0x%x\n", addr);
  for (tag = (struct multiboot_tag *)(addr + 8);
       tag->type != MULTIBOOT_TAG_TYPE_END;
       tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7)))
  {
    LOG("Тег: %s, Размер: %u Байт\n", __mb2_htags_str[tag->type], tag->size);
    switch (tag->type)
    {
    case MULTIBOOT_TAG_TYPE_CMDLINE:
      LOG(" -> КМД_СТР: = %s\n",
               ((struct multiboot_tag_string *)tag)->string);
      break;
    case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
      LOG(" -> Название бутлоудера = %s\n",
               ((struct multiboot_tag_string *)tag)->string);
      break;
    case MULTIBOOT_TAG_TYPE_MODULE:
      LOG(" -> Модуль расположен с 0x%x до 0x%x. КМД_СТР: %s\n",
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
      LOG(" -> Устройство с которого произведена загрузка: 0x%x,%u,%u\n",
               ((struct multiboot_tag_bootdev *)tag)->biosdev,
               ((struct multiboot_tag_bootdev *)tag)->slice,
               ((struct multiboot_tag_bootdev *)tag)->part);
      break;
    case MULTIBOOT_TAG_TYPE_MMAP:
    {
      multiboot_memory_map_t *mmap;

      LOG(" -> Карта памяти\n");

      for (mmap = ((struct multiboot_tag_mmap *)tag)->entries;
           (multiboot_uint8_t *)mmap < (multiboot_uint8_t *)tag + tag->size;
           mmap = (multiboot_memory_map_t *)((unsigned long)mmap + ((struct multiboot_tag_mmap *)tag)->entry_size))
        LOG(" --> Расположен по адресу 0x%x%x,"
                 " Размер = 0x%x%x, Тип = 0x%x\n",
                 (unsigned)(mmap->addr >> 32),
                 (unsigned)(mmap->addr & 0xffffffff),
                 (unsigned)(mmap->len >> 32),
                 (unsigned)(mmap->len & 0xffffffff),
                 (unsigned)mmap->type);
    }
      _mmap_mb2 = (struct multiboot_tag_mmap *)tag;
      break;
    case MULTIBOOT_TAG_TYPE_FRAMEBUFFER:
    {
      multiboot_uint32_t color;
      unsigned i;
      struct multiboot_tag_framebuffer *tagfb = (struct multiboot_tag_framebuffer *)tag;
      void *fb = (void *)(unsigned long)tagfb->common.framebuffer_addr;
      LOG(" -> Тип фреймбуфера: %s\n",
               __mb2_htag_fb_str[tagfb->common.framebuffer_type]);
      LOG(" -> Разрешение %ux%u px, BPP: %u Бит\n",
               tagfb->common.framebuffer_width,
               tagfb->common.framebuffer_height,
               tagfb->common.framebuffer_bpp);
      LOG(" -> Расположен по адресу 0x%x\n", tagfb->common.framebuffer_addr);
      info->framebuf = *tagfb;
      break;
    }
    }
  }
  tag = (struct multiboot_tag *)((multiboot_uint8_t *)tag + ((tag->size + 7) & ~7));
  LOG(" MBI занимает %u Байт\n", (unsigned)tag - addr);
  LOG("[INF ] Чтение данных загрузчика завершено.\n");
  return 1;
}