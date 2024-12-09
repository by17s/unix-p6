#include <mem.h>  // Include memory management functions
#include <kernel.h>  // Include kernel-related functions and definitions

#define BLOCK_SIZE 4096 // Define block size as 4 KB
#define PAGE_SIZE 4096 // Define page size as 4 KB
#define UINT32BITS (32) // Define number of bits in a uint32_t

#define BSIZE sizeof(uint32_t) // Size of uint32_t

// Calculate total size needed for blocks and bitmaps
#define calcsize(blocks, bitmaps) ((blocks) * BLOCK_SIZE + (bitmaps) * sizeof(uint32_t))

// Bit manipulation macros
#define test_bit(val, bit) ((val & bit) != 0) // Test if a specific bit is set
#define set_bit(val, bit) val |= bit // Set a specific bit
#define clear_bit(val, bit) val &= ~bit // Clear a specific bit
#define next_bit(bit) bit <<= 1; // Shift to the next bit
#define calc_pageaddr(bitmapi, bit) (((bitmapi) * UINT32BITS + bit) * BLOCK_SIZE) // Calculate page address based on bitmap index and bit
#define test_bitnumber(num, bitn) ((num & (1 << bitn)) != 0) // Test if a specific bit number is set

// Structure representing a memory block
typedef struct mem_block_s {
    struct mem_block_s* next; // Pointer to the next memory block
    size_t task_id; // ID of the task using this block
    size_t free; // Flag indicating if the block is free
    size_t size; // Size of the block
    uint8_t* data[0]; // Flexible array member for block data
} mem_block_t;

// External variables for dynamic memory management
extern size_t m_dyn_available, m_dyn_total; 
extern void *m_dyn_base, *m_dyn_limit;
extern mem_block_t* m_dyn_first_block;

// Function to merge adjacent free memory blocks
static void merge_blocks(mem_block_t *start) {
	if (!start->free) return; // If the block is not free, return

	mem_block_t *block = start;
	while (block->next && block->next->free) { // Merge with next free blocks
		block->size += block->next->size + sizeof(mem_block_t); // Update size

		if (block->next->next) {
			block->next = block->next->next; // Link to the next non-free block
			continue;
		}
		block->next = NULL; // End of the list reached
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

// Function to allocate memory without alignment requirement
void* kmalloc(size_t size) {
    return kaligned_alloc(size, 1); // Call aligned allocation with alignment of 1 byte
}

// Function to free allocated memory blocks
void kfree(void* ptr) {
    if(ptr == NULL) return; // Do nothing if pointer is NULL

    mem_block_t *block = m_dyn_first_block, *prev = NULL;
	while (block != NULL) {
		if (block->data == ptr) { 
            // If the pointer matches the data in a memory block
			block->free = 1; 
			merge_blocks(prev ? prev : block); // Merge with previous blocks if applicable

            mem_block_t *blk = m_dyn_first_block;
			while (blk) {
                merge_blocks(blk); // Merge all blocks in the list after freeing
                blk = blk->next;
            }
			return;
		}
		prev = block;
		block = block->next;
	}
}
