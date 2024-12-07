#pragma once

struct spinlock_s {
    volatile int lock;
};

typedef struct spinlock_s* spinlock_t;

extern void spin_lock(spinlock_t lock);

extern void spin_unlock(spinlock_t lock);

#define spin_init(lk) lk->lock = 0; 