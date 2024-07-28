#ifndef __MEM_H_
#define __MEM_H_

#include <tools.h>
#include <log.h>

#define MEM_FRAME_SIZE 0x1000         // 4KB
#define MEM_DYNAMIC_MIN_SIZE 0x800000 // 8MB

void *alloc_frames(size_t count);

void free_frames(void *addr, size_t count);

void* kaligned_alloc(size_t size, size_t alignment);

void* kmalloc(size_t size);

void kfree(void* ptr);

int mm_init(void *addr, size_t size);

#endif //__MEM_H_