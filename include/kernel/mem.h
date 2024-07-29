#ifndef __MEM_H_
#define __MEM_H_

#include <tools.h>
#include <log.h>
#include <multiboot2.h>

#define MEM_FRAME_SIZE 0x1000         // 4KB
#define MEM_DYNAMIC_MIN_SIZE 0x100000 // 8MB

#define BITMAP_STACK_SIZE 32

#define MEM_BLOCK_AVAILABLE              1
#define MEM_BLOCK_RESERVED               2
#define MEM_BLOCK_ACPI_RECLAIMABLE       3
#define MEM_BLOCK_NVS                    4
#define MEM_BLOCK_BADRAM                 5

typedef struct
{
    void* next;
    uint32_t search_next_startblock;
    uint32_t* first_bitmap;
    uint32_t blocks_count;
    void* first_block;
} bitmap_list_t;

struct memblock_entry
{
    uint32_t size;
	uint32_t addr, addr_high;
    // You can also use: unsigned long long int base_addr; if supported.
	uint32_t len, len_high;
    // You can also use: unsigned long long int length; if supported.
	uint32_t type;
} __attribute__((packed));
typedef struct memblock_entry memblock_entry_t;

void mm_init(void* mmap_addr, uint32_t mmap_length);
bitmap_list_t* mm_initmemblock(memblock_entry_t* block);
void* fralloc(uint32_t count, uint32_t align_up);
void frfree(void* addr, uint32_t count);

void* kmalloc(size_t size);
void kfree(void* ptr);

#endif //__MEM_H_