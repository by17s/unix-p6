#include <log.h>

#include <drivers/tvga.h>

#include <x86.h>

// 0 - serial; 1 - terminal
int __log_out = 0;

void __tvga_putch(char c)
{
    tvga_write(&c, 1);
}

void log_set_output(int out) {
    __log_out = out;
}

void LOG(const char* __fmt, ...) {
    va_list args;
    va_start(args, __fmt);
    if(__log_out)
        __doprnt(__fmt, args, __tvga_putch);
    else
        __doprnt(__fmt, args, com_write);
    va_end(args);
}