#include <mem.h>
#include <kernel.h>

#define BLOCK_SIZE 4096 // 4 KB
#define PAGE_SIZE 4096 // 4 KB
#define UINT32BITS (32)

#define BSIZE sizeof(uint32_t)

#define calcsize(blocks, bitmaps) ((blocks) * BLOCK_SIZE + (bitmaps) * sizeof(uint32_t))

#define test_bit(val, bit) ((val & bit) != 0)
#define set_bit(val, bit) val |= bit
#define clear_bit(val, bit) val &= ~bit
#define next_bit(bit) bit <<= 1;
#define calc_pageaddr(bitmapi, bit) (((bitmapi) * UINT32BITS + bit) * BLOCK_SIZE)
#define test_bitnumber(num, bitn) ((num & (1 << bitn)) != 0)

typedef struct mem_block_s {
    struct mem_block_s* next;
    size_t task_id;
    size_t free;
    size_t size;
    uint8_t* data[0];
} mem_block_t;

extern size_t m_dyn_available, m_dyn_total; 
extern void *m_dyn_base, *m_dyn_limit;
extern mem_block_t* m_dyn_first_block;


static void merge_blocks(mem_block_t *start) {
	if (!start->free) return;
	mem_block_t *block = start;
	while (block->next && block->next->free) {
		block->size += block->next->size + sizeof(mem_block_t);

		if (block->next->next) {
			block->next = block->next->next;
			continue;
		}
		block->next = NULL;
	}
}


void* kaligned_alloc(size_t size, size_t alignment) {
    if(size == 0) return NULL;

    if(size > m_dyn_available) {
        //PANIC
    }

    mem_block_t *block = m_dyn_first_block;

	while (block) {
		if (block->free && block->size >= (alignment + sizeof(mem_block_t) + size)) {
			void *addr = block->data + alignment - 1;
			addr -= (uintptr_t)addr % alignment + sizeof(mem_block_t);
			mem_block_t *second = (mem_block_t *)addr;

			mem_block_t *third = (mem_block_t *)(second->data + size);
			tmemset(third, 0, sizeof(mem_block_t));

			third->size = block->size - (third->data - block->data);
			third->next = block->next;
			third->free = 1;

			second->size = size;
			second->next = third;
			second->free = 0;
			second->task_id = 0;

			if (block != second) {
				block->next = second;
				block->size = (uintptr_t)second - (uintptr_t)block->data;
				block->free = 1;
			}

			return second->data;
		}

		block = block->next;
	}

	return NULL;
}

void* kmalloc(size_t size) {
    return kaligned_alloc(size, 1);
}

void kfree(void* ptr) {
    if(ptr == NULL) return;

    mem_block_t *block = m_dyn_first_block, *prev = NULL;
	while (block != NULL) {
		if (block->data == ptr) {
			block->free = 1;
			merge_blocks(prev ? prev : block);

            mem_block_t *blk = m_dyn_first_block;
			while (blk) {
                merge_blocks(blk);
                blk = blk->next;
            }
			return;
		}
		prev = block;
		block = block->next;
	}
    
}