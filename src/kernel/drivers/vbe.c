#include <drivers/vbe.h>

struct __vbe_info_s {
    void *_fb;
    uint32_t* fb;

    int h, w;
    int bpp;
    enum vbe_color_palette palette;
} __vbe_info;

int vbe_init(void* fb, int w, int h, int bpp, enum vbe_color_palette p) {
    __vbe_info._fb = fb;
    __vbe_info.fb = (uint32_t*)fb;
    __vbe_info.w = w;
    __vbe_info.h = h;
    __vbe_info.bpp = bpp;
    __vbe_info.palette = p;
}

void __vbe_clear(uint32_t color) {
    tmemsetd(__vbe_info._fb, color, __vbe_info.w *  __vbe_info.h);
}

void __vbe_fill_line(uint32_t color, uint32_t ln) {
    tmemsetd(&__vbe_info.fb[ln * __vbe_info.w], color, __vbe_info.w);
}

int vbe_sendcmd(cmd_t cmd, ...) {
    va_list args;
    va_start(args, cmd);
    
    switch (cmd)
    {
    case VBE_CLEAR:
        //u32
        __vbe_clear(va_arg(args, uint32_t));
        break;
    case VBE_FILL_LINE:
        //u32, u32
       __vbe_fill_line(va_arg(args, uint32_t), va_arg(args, uint32_t));
        break;
    case VBE_GET_RES:
        //i32*, i32*
        *(va_arg(args, int*)) = __vbe_info.w;
        *(va_arg(args, int*)) = __vbe_info.h;
        break;
    default:
        va_end(args);
        return 0;
        break;
    }
    va_end(args);
    return 1;
}

int vbe_set_px(size_t pos, uint32_t data) {
    __vbe_info.fb[pos] = data;
}

uint32_t vbe_get_px(size_t pos) {
    return __vbe_info.fb[pos];
}