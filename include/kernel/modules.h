#ifndef __MODULES_H_
#define __MODULES_H_

#include <tools.h>
#include <log.h>
#include <multiboot2.h>

typedef struct {
    char magic[5];
    char ver[3];
    char name[32];
    uint32_t flags;
    uint8_t data[0];
} module_t;

void module_add_to_list(struct multiboot_tag_module *m);

module_t *module_get(size_t idx);

module_t *module_search(char *name);

int modules_init(void);

#endif //__MODULES_H_