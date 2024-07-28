#include <mem.h>

typedef struct mem_block_s {
    struct mem_block_s* next;
    size_t task_id;
    size_t free;
    size_t size;
    uint8_t* data[0];
} mem_block_t;

uint8_t *m_bitmap;
size_t m_frames_available, m_frames_limit = 0;
void *m_frames_base;

size_t m_available = 0;

size_t m_dyn_available, m_dyn_total = 0; 
void *m_dyn_base, *m_dyn_limit = NULL;
mem_block_t* m_dyn_first_block = NULL;

void *alloc_frames(size_t count)
{
    if (count == 0)
        return NULL;

    size_t wanted = count;
    int at = 0;
    for (size_t i = 0; i < m_frames_limit; i++)
    {
        if (!BIT_GET(m_bitmap, i))
        {
            if (at == 0)
                at = i;
            wanted--;
            if (wanted == 0)
            {
                for (size_t b = at; b < at + count; b++)
                {
                    BIT_SET(m_bitmap, b);
                }
                return m_frames_base + (at * MEM_FRAME_SIZE);
            }
        }
        else if (wanted > 0)
        {
            wanted = count;
            at = 0;
        }
    }
    return NULL;
}

void free_frames(void *addr, size_t count)
{
    if (count == 0 || addr == NULL || addr < m_frames_base)
        return;

    size_t frame = (size_t)(addr - m_frames_base) / MEM_FRAME_SIZE;
    for (size_t i = frame; i < frame + count; i++)
        BIT_CLEAR(m_bitmap, i);
}


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

int mm_init(void *addr, size_t size)
{
    LOG("[INF ] Initializing the memory manager.\n");
    size_t base = (size_t)addr;

    if (size < MEM_DYNAMIC_MIN_SIZE * 2)
        return -1;

    m_frames_limit = (size - MEM_DYNAMIC_MIN_SIZE) / MEM_FRAME_SIZE - 2;
    LOG(" -> Total frames: %u\n", m_frames_limit);

    size_t bitmap_size = m_frames_limit / 8;
    LOG(" -> Bitmap size (in bytes): %u\n", bitmap_size);

    m_bitmap = (uint8_t *)addr;
    m_frames_base = addr + bitmap_size;
    m_frames_base += MEM_FRAME_SIZE - ((size_t)m_frames_base % MEM_FRAME_SIZE);
    //LOG(" -> Frames base: %x\n", m_frames_base);

    // clear bitmap
    for (size_t i = 0; i < m_frames_limit + 1; i++)
        BIT_CLEAR(m_bitmap, i);

    
    m_dyn_base = m_frames_base + (m_frames_limit * MEM_FRAME_SIZE);
    m_dyn_total = size - (m_dyn_base - addr);

    m_dyn_first_block = (mem_block_t*)m_dyn_base;
    m_dyn_first_block->free = 1;
    m_dyn_first_block->size = m_dyn_total - sizeof(mem_block_t);
    m_dyn_first_block->next = NULL;

    //LOG(" -> Dynamic memory size (in bytes): %zx\n",  m_dyn_total);  
    LOG("[INF ] Memory manager initialization complete.\n");
}