#pragma once

#include <tools.h>

enum vbe_color_palette {
    vbe_color_palette_Monochrome, // white and black
    vbe_color_palette_16Colors, //Bios colors in text mode
    vbe_color_palette_RGB, // RGB
};

int vbe_init(void* fb, int w, int h, int bpp, enum vbe_color_palette p);

int vbe_sendcmd(cmd_t cmd, ...);