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

bitmap_list_t bitmap_stack[BITMAP_STACK_SIZE];
uint32_t bitmap_stack_top = 0;

bitmap_list_t* firstblock = NULL;

uint32_t align_up(uint32_t address, uint32_t align) {
    if (align <= 0) {
        return address;
    }
    uint32_t remainder = address % align;
    if (remainder == 0) {
        return address;
    }
    return address + align - remainder;
}

bitmap_list_t* alloc_bitmap_list()
{
	if(bitmap_stack_top >= BITMAP_STACK_SIZE)
		return NULL;
	
	return &bitmap_stack[bitmap_stack_top++];
}

void calc_blocks_and_bitmaps(uint32_t size, uint32_t* num_blocks, uint32_t* num_bitmaps)
{
    uint32_t blocks = size / BLOCK_SIZE;
    uint32_t bitmaps = blocks / UINT32BITS + 1;

    while (1)
    {
        if (calcsize(blocks, bitmaps) <= size)
            break;

        blocks--;
        bitmaps = blocks / UINT32BITS + 1;
    }

    *num_blocks = blocks;
    *num_bitmaps = bitmaps;
}

static inline void block_setstart(memblock_entry_t* block, uint32_t new)
{
    int32_t old = new - block->addr;
    block->addr = new;
    block->len = (old > 0) ? (block->len - (uint32_t)old) : ((block->len - (uint32_t)(-old)));
}

static inline void block_addstart(memblock_entry_t* block, uint32_t inc)
{
	block_setstart(block, block->addr + inc);
}

static inline void block_setend(memblock_entry_t* block, uint32_t new)
{
    block->len = new - block->addr;
}

static inline uint32_t block_getend(memblock_entry_t* block)
{
    return block->addr + block->len;
}

static inline uint32_t is_block_under1mb(memblock_entry_t* block)
{
    return block_getend(block) < 0x100000;
}

static inline uint32_t is_partofblock_under1mb(memblock_entry_t* block)
{
    return block_getend(block) >= 0x100000 && block->addr < 0x100000;
}

bitmap_list_t* mm_initmemblock(memblock_entry_t* block)
{
    //We dont allow memory under 1MB
    if(is_block_under1mb(block))
    {
        block->type = MEM_BLOCK_RESERVED;
        printf("Chunk is under 1MB\n");
        return NULL;
    }

    if(is_partofblock_under1mb(block)) // and if block end is under 1 mb
    {
        block_setstart(block, 0x100000);
    }
    
    uint32_t blockend = block_getend(block);

    int b = block->addr < (uint32_t)&__kernel_start;
    int t = blockend > (uint32_t)&__kernel_end;

    //Cut conflict between kernel memory and block memmory
    if((b && t) || t)
    {
        block_setstart(block, (uint32_t)&__kernel_end);
    }
    else if(b)
    {
        block_setend(block, (uint32_t)&__kernel_start);
    }

	if(block->len >= MEM_DYNAMIC_MIN_SIZE && !m_dyn_first_block)
	{
		m_dyn_base = block->addr;
    	m_dyn_total = MEM_DYNAMIC_MIN_SIZE;

    	m_dyn_first_block = (mem_block_t*)m_dyn_base;
    	m_dyn_first_block->free = 1;
    	m_dyn_first_block->size = m_dyn_total - sizeof(mem_block_t);
    	m_dyn_first_block->next = NULL;

		block_addstart(block, MEM_DYNAMIC_MIN_SIZE);
	}

    //Align memory on page
    uint32_t alignedaddr = align_up(block->addr, PAGE_SIZE);

    if(alignedaddr >= block->addr + block->len)
    {
        //🙂
        LOG("Cut so much that no memory there is left\n");
        block->type = MEM_BLOCK_RESERVED;
        return NULL;
    }

    block_setstart(block, alignedaddr);

    //Clear block with zero
    memset((void*)block->addr, 0, block->len);

    uint32_t blocks = 0;
    uint32_t bitmaps = 0;

    calc_blocks_and_bitmaps(block->len, &blocks, &bitmaps);
    
	bitmap_list_t* entry = alloc_bitmap_list();
    entry->blocks_count = blocks;
    entry->first_block = (void*)((uint32_t)block->addr);
    entry->first_bitmap = (uint32_t*)((uint32_t)block->addr + blocks * BLOCK_SIZE);
    entry->search_next_startblock = 0;
    entry->next = NULL;

    if(firstblock == NULL)
        firstblock = entry;

    return entry;
}

void* fralloc(uint32_t count, uint32_t align_up)
{
    if(count == 0 || count > 32)
        return NULL;
    
    if(align_up == 0)
        align_up = 1;

    bitmap_list_t* curblock = firstblock;
    while(1)
    {
        const uint32_t repeatcount = curblock->blocks_count / UINT32BITS + 1;
        char isnotset_snsb = 1;
        for(uint32_t i = curblock->search_next_startblock; i < repeatcount; i++)
        {
            uint32_t* bitmap = &(curblock->first_bitmap)[i];

            if(*bitmap == 0xffffFFFF)
                continue;

            uint32_t bit = 1;
            uint32_t streak = 0;
            uint32_t firstj = 0;
            for(uint32_t j = 0; j < UINT32BITS; j++)
            {
                if(!streak) {
                    if(32 - j < count) {
                        break;
                    }

                    if(j % align_up != 0) {
                        continue;
                    }
                }

                if(!test_bit(*bitmap, bit))
                {
                    if(streak == 0)
                    {
                        firstj = j;
                    }

                    streak++;

                    if(isnotset_snsb)
                    {
                        curblock->search_next_startblock = i;
                        isnotset_snsb = 0;
                    }

                    if(streak == count)
                    {
                        j -= streak - 1;

                        bit = 1 << j;
                        for (uint32_t k = 0; k < streak; k++)
                        {
                            set_bit(*bitmap, bit);
                            next_bit(bit);
                        }
                        return (void*)((uintptr_t)(curblock->first_block) + calc_pageaddr(i, firstj));
                    }
                }
                else
                {
                    streak = 0;
                }
                next_bit(bit);
            }
        }

        if(curblock->next == NULL) {
            return NULL;
        }
        
        curblock = curblock->next;
    }
    return NULL;
}

void frfree(void* addr, uint32_t count)
{
    uint32_t addrint = (uint32_t)addr;

    if(count == 0 || count > 32)
        return;

    bitmap_list_t* curblock = firstblock;
    while(1)
    {
        if(addrint >= (uint32_t)curblock->first_block && addrint <= (uint32_t)curblock->first_block + curblock->blocks_count)
        {
            addrint -= (uint32_t)curblock->first_block;
            addrint /= BLOCK_SIZE;
            uint32_t bitn = addrint % 31;
            addrint -= bitn;
            uint32_t bitmapn = addrint / 32;
            uint32_t* bitmap = &(curblock->first_bitmap)[bitmapn];
            uint32_t bit = 1 << bitn;

            if(32 - bitn < count) {
                return;
            }

            for (uint32_t k = 0; k < count; k++)
            {
                clear_bit(*bitmap, bit);
                next_bit(bit);
            }
            return;
        }

        if(curblock->next == NULL) {
            break;
        }
        
        curblock = curblock->next;
    }
    LOG("Cant free block with addr %x and size %i\n", addr, count);
    return;
}

void mm_init(void* mmap_addr, uint32_t mmap_length)
{
    bitmap_list_t* newblock = NULL;
    for (size_t i = 0; i < mmap_length; i += sizeof(memblock_entry_t))
    {
        memblock_entry_t* me = (memblock_entry_t*)(mmap_addr + i);

        if(me->type == MEM_BLOCK_AVAILABLE)
        {
            bitmap_list_t* oldblock = newblock;
            newblock = mm_initmemblock(me);

            if(oldblock) {
                oldblock->next = newblock;
            }

            LOG("addr = %x, len = %x, size = %x, type = %i, ret = %x\n", (uint32_t)me->addr, (uint32_t)me->len, me->size, me->type, newblock);
        }
    }
}

size_t m_dyn_available, m_dyn_total = 0; 
void *m_dyn_base, *m_dyn_limit = NULL;
mem_block_t* m_dyn_first_block = NULL;

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