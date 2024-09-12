#ifndef __MODULES_H_
#define __MODULES_H_

#include <tools.h>
#include <log.h>
#include <multiboot2.h>

typedef struct multiboot_tag_module module_t;

void module_add_to_list(struct multiboot_tag_module *m);

module_t *module_get(size_t idx);

module_t *module_search(char *name);

int modules_init(void);

#endif //__MODULES_H_