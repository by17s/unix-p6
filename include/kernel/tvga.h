#include <stdint.h>

void tvga_init(void);
void tvga_command(uint32_t cmd, uint32_t arg1, uint32_t arg2, uint32_t arg3);
void tvga_write(const char* str, uint32_t count);