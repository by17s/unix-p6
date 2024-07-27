#include <kernel.h>

#define mbui64 multiboot_uint64_t
#define mbui32 multiboot_uint32_t

static long long sqr(unsigned x, unsigned y, unsigned x1, unsigned y1)
{
	long long xx = x - x1;
	long long yy = y - y1;
	return xx * xx + yy * yy;
}

static unsigned div(unsigned a, unsigned b)
{
	unsigned r = 0;
	while (a > b) {
		a -= b;
		++r;
	}
	return r;
}

static unsigned char calc_color(unsigned x, unsigned y, unsigned tx, unsigned ty, unsigned coef)
{
	unsigned dist2 = sqr(x, y, tx, ty);
	unsigned col = div(dist2 * 256, coef);
	if (col > 255)
		col = 255;
	return (unsigned char)col;
}

void kmain(multiboot_uint32_t magic, unsigned tags_addr)
{
	struct sys_info info;
    sys_init(magic, tags_addr, &info);
    
#define fbc (info.framebuf.common)
	
	if (fbc.framebuffer_type != MULTIBOOT_FRAMEBUFFER_TYPE_RGB)
		goto kend;

#define fbh fbc.framebuffer_height
#define fbw fbc.framebuffer_width
	
	unsigned bpp = fbc.framebuffer_bpp / 8; // bytes per pixel
	unsigned char *p = (unsigned char *)fbc.framebuffer_addr;
	
	unsigned redx = 0;
	unsigned redy = fbh;
	unsigned greenx = fbw / 2;
	unsigned greeny = 0;
	unsigned bluex = fbw;
	unsigned bluey = fbh;
	
	for (unsigned h = 0; h < fbh; ++h)
	{
		for (unsigned w = 0; w < fbw; ++w)
		{
			unsigned char *px = p + (fbc.framebuffer_pitch * h + w * bpp);
			unsigned red = calc_color(w, h, redx, redy, fbw * fbw);
			unsigned green = calc_color(w, h, greenx, greeny, fbw * fbw);
			unsigned blue = calc_color(w, h, bluex, bluey, fbw * fbw);
			
			px[0] = blue;
			px[1] = green;
			px[2] = red;
		} 
	}

	for (;;);
kend:
}