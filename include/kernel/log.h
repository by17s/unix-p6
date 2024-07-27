#ifndef __LOG_H_
#define __LOG_H_

#include <tools.h>

void __log_com(const char* __fmt, ...);
void __log_term(const char* __fmt, ...);

void LOG(const char* __fmt, ...);

#endif //__LOG_H_