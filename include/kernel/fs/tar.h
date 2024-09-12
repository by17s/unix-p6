#ifndef __TAR_H_
#define __TAR_H_

#include <tools.h>
#include <mem.h>

#include "file.h"

typedef struct {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
} tar_header_t;

typedef struct {
    char* ptr;
    size_t size;
} tar_t;

tar_t* tar_init(char* tar_ptr, size_t tar_size);

tar_t tar_get_file(tar_t* tar, char* name);

#endif //__TAR_H_