#ifndef __FILE_H_
#define __FILE_H_

#include <tools.h>
#include <mem.h>

typedef struct {
    size_t size;
    uint32_t mode, uid, gid;
} file_info_t;

#endif //__FILE_H_