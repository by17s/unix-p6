#include <fs/tar.h>

tar_t* tar_init(char* tar_ptr, size_t tar_size) {
    tar_t *tar = (tar_t*)kmalloc(sizeof(tar_t));
    tar->ptr = tar_ptr;
    tar->size = tar_size;
    return tar;
}

uint32_t tar_get_size(char *in) {
    uint32_t size = 0;
    uint32_t j;
    uint32_t count = 1;

    for (j = 11; j > 0; j--, count *= 8)
        size += ((in[j - 1] - '0') * count);

    return size;
}

tar_t tar_get_file(tar_t* tar, char* name) {
    size_t offset = 512;
    tar_header_t* th;
    tar_t file = {
        .size = 0,
        .ptr = 0
    };

    for(;;) {
        th = (tar_header_t*)(tar->ptr + offset);
        if(th->name[0] == '\0') break;

        size_t size = tar_get_size(th->size);
        
        if(strncmp(th->name, name, 100) == 0) {
            file.size = size;
            file.ptr = tar->ptr + offset + 512;
            return file;
        }
        
        offset += ((size / 512) + 1) * 512;

        if(size % 512) {
            offset += 512;
        }
    }
    
    return file;
}