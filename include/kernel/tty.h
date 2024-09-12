#ifndef __TTY_H_
#define __TTY_H_

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

typedef struct tty_s {
    uint32_t  flags;
    char* buff;
    size_t buff_size;
    size_t buff_pos;
    uint32_t timeout;
} tty_t;

int tty_write(tty_t *tty, char* data, size_t len);
int tty_setattr(tty_t *tty, char attr, uint32_t value);

#endif //__TTY_H_