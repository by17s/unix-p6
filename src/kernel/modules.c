#include <modules.h>

#define MODULES_MAX_LOAD 256

struct multiboot_tag_module *__modules[MODULES_MAX_LOAD];
size_t __modules_count = 0;

void module_add_to_list(struct multiboot_tag_module *m) {
    if(__modules_count + 1 >= MODULES_MAX_LOAD);
    __modules[__modules_count] = m;
    __modules_count++;
}

module_t *module_get(size_t idx) {
    if(idx >= __modules_count) return NULL;
    return (module_t*)__modules[idx];
}

module_t *module_search(char *name) {
    return NULL;
}

int modules_init(void)
{
    LOG("[INF ] Modules loaded: %d\n", __modules_counter);
    return 0;
}
