#include <drivers/tvga.h>

#include <stdint.h>
#include <stddef.h>
#include <tools.h>
#include <x86.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BUFFER_ADDRESS 0xB8000
#define DEFAULT_COLOR 0x0F
#define TABSPACES 4

static uint16_t* const tvga_vbuffer = (uint16_t*)VGA_BUFFER_ADDRESS;
static uint8_t tvga_curattr = DEFAULT_COLOR;
static uint16_t cursor_pos = 0;

static inline uint16_t attr_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t) uc | (uint16_t) color << 8;
}

void tvga_updatecursorio(uint16_t pos)
{
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

static void tvga_updatecursor(uint32_t position)
{
    cursor_pos = position;
    tvga_updatecursorio(cursor_pos);
}

static void tvga_setattr(uint8_t color)
{
    tvga_curattr = color;
}

static void tvga_clr()
{
    tmemsetw(tvga_vbuffer, attr_entry(' ', tvga_curattr), VGA_HEIGHT * VGA_WIDTH);
    cursor_pos = 0;
}

static void tvga_reset()
{
    tvga_setattr(DEFAULT_COLOR);
    tvga_clr();
}

void tvga_init()
{
    tvga_reset();
}

void tvga_command(cmd_t cmd, uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
    switch(cmd) {
        case VGA_CMD_SETCURSOR:
            tvga_updatecursor(arg1);
            break;
        case VGA_CMD_SETCOLOR:
            tvga_setattr((uint8_t)arg1);
            break;
        case VGA_CMD_CLEAR:
            tvga_clr();
            break;
        default:
            break;
    }
}

static void tvga_linefeed()
{
    cursor_pos = (cursor_pos / VGA_WIDTH + 1) * VGA_WIDTH;
}

static void tvga_carriagereturn()
{
    cursor_pos -= (cursor_pos % VGA_WIDTH);
}

static void tvga_tab()
{
    cursor_pos = cursor_pos - cursor_pos % TABSPACES + TABSPACES;
}

static void tvga_backspace()
{
    if (cursor_pos > 0) {
        cursor_pos--;
    }
}

static void tvga_scrollscreen()
{
    tmemcpyw(tvga_vbuffer, tvga_vbuffer + VGA_WIDTH, (VGA_HEIGHT - 1) * VGA_WIDTH);
    
    tmemsetw(tvga_vbuffer + (VGA_HEIGHT - 1) * VGA_WIDTH, attr_entry(' ', tvga_curattr), VGA_WIDTH);
    
    cursor_pos -= VGA_WIDTH;
}

static void tvga_putch(char c)
{
    switch(c) {
        case '\n':
            tvga_linefeed();
            break;
        case '\r':
            tvga_carriagereturn();
            break;
        case '\t':
            tvga_tab();
            break;
        case '\b':
            tvga_backspace();
            break;
        default:
            tvga_vbuffer[cursor_pos++] = attr_entry(c, tvga_curattr);
            break;
    }

    if (cursor_pos >= VGA_WIDTH * VGA_HEIGHT) {
        tvga_scrollscreen();
    }

    tvga_updatecursor(cursor_pos);
}

void tvga_write(const char* str, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        tvga_putch(str[i]);
    }
}
