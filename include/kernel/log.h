#ifndef __LOG_H_
#define __LOG_H_

#include <tools.h>

void __log_com(const char* __fmt, ...);
void __log_term(const char* __fmt, ...);

#ifdef DEBUG 
    #define LOG __log_com
#else
    #define LOG __log_term
#endif

#endif //__LOG_H_