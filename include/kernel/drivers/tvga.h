#pragma once

#include <stdint.h>

#include <tools.h>

enum vga_cmd
{
    VGA_CMD_SETCURSOR,
    VGA_CMD_SETCOLOR,
    VGA_CMD_CLEAR
};

void tvga_init(void);
void tvga_command(cmd_t cmd, uint32_t arg1, uint32_t arg2, uint32_t arg3);
void tvga_write(const char* str, uint32_t count);