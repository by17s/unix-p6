#include <log.h>

#include <arch.h>

void __log_com(const char* __fmt, ...) {
    va_list args;
    va_start(args, __fmt);
    __doprnt(__fmt, args, com_write);
    va_end(args);
}

void __log_term(const char* __fmt, ...) {
    va_list args;
    va_start(args, __fmt);
    //__doprnt(__fmt, args, com_write);
    va_end(args);
}