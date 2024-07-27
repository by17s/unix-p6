#pragma once

#include <tools.h>

#define VBE_CLEAR 0
#define VBE_FILL_LINE 2
#define VBE_DRAW_BUFFER 3
#define VBE_DRAW_TEXTURE 4
#define VBE_GET_RES 5
#define VBE_SET_FB 6
#define VBE_GET_FB 7

enum vbe_color_palette {
    vbe_color_palette_Monochrome, // white and black
    vbe_color_palette_16Colors, //Bios colors in text mode
    vbe_color_palette_RGB, // RGB
};

int vbe_init(void* fb, int w, int h, int bpp, enum vbe_color_palette p);

int vbe_sendcmd(cmd_t cmd, ...);

int vbe_set_px(size_t pos, uint32_t data);

uint32_t vbe_get_px(size_t pos);