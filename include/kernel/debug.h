#ifndef __DEBUG_H_
#define __DEBUG_H_

#include <log.h>
#include <stdarg.h>

#define DEBUG 

#define ERROR
#define WARNING
#define NOTICE


#ifdef DEBUG
# define debug_log(m, ...) do { LOG(__FILE__ ":%u [" #m "] ", __LINE__); LOG(__VA_ARGS__); LOG("\n"); } while (0);
#else
# define debug_log(m, ...)
#endif

#endif //__DEBUG_H_