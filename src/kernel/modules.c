#include <modules.h>

#define MODULES_MAX_LOAD 256

struct multiboot_tag_module *__modules[MODULES_MAX_LOAD];
size_t __modules_counter = 0;

void module_add_to_list(struct multiboot_tag_module *m) {
    if(__modules_counter + 1 >= MODULES_MAX_LOAD);
    __modules[__modules_counter] = m;
    __modules_counter++;
}

module_t *module_get(size_t idx) {
    if(idx >= __modules_counter) return NULL;
    
    return (module_t*)__modules[idx];
}

module_t *module_search(char *name) {
    for (size_t i = 0; i < __modules_counter; i++) {
        module_t *mdl = (module_t*)__modules[__modules_counter];
        if(tmemcmp(mdl->magic, "uxmdl", 5))
        {
            if(check_eq_of_strings(mdl->name, name)) {
                return mdl;
            }
        }
    }
    return NULL;
}

int modules_init(void)
{
    LOG("[INF ] Modules loaded: %d\n", __modules_counter);
    return 0;
}
