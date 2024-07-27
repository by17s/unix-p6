#include <log.h>

#include <drivers/tvga.h>

#include <x86.h>

void __tvga_putch(char c)
{
    tvga_write(&c, 1);
}

void __log_com(const char* __fmt, ...) {
    va_list args;
    va_start(args, __fmt);
    __doprnt(__fmt, args, com_write);
    __doprnt(__fmt, args, __tvga_putch);
    va_end(args);
}

void __log_term(const char* __fmt, ...) {
    va_list args;
    va_start(args, __fmt);
    __doprnt(__fmt, args, __tvga_putch);
    va_end(args);
}