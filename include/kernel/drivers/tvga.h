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
int tvga_command(cmd_t cmd, ...);
void tvga_write(const char* str, uint32_t count);