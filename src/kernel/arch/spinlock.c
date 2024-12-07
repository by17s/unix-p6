#include <spinlock.h>

static inline int _atomic_exchange(volatile int *ptr, int new_value) {
    int old_value;
    __asm__ volatile (
        "xchg %0, %1"
        : "=r"(old_value), "+m"(*ptr)
        : "0"(new_value)             
        : "memory"                   
    );
    return old_value;
}

void spin_lock(spinlock_t lock) {
    while (_atomic_exchange(&lock->lock, 1)) {

    }
}

void spin_unlock(spinlock_t lock) {
    lock->lock = 0;
}