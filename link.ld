OUTPUT_ARCH(i386);
OUTPUT_FORMAT(elf32-i386);
ENTRY(_start)

SECTIONS
{
    . = 0x100000;
    __kernel_start = .;

	.text BLOCK(4K) : ALIGN(4K)
	{
		*(.mbheader)
		*(.text)
	}

	.rodata BLOCK(4K) : ALIGN(4K)
	{
		*(.rodata)
	}

	.data BLOCK(4K) : ALIGN(4K)
	{
		*(.data)
	}

	.bss BLOCK(4K) : ALIGN(4K)
	{
		*(COMMON)
		*(.bss)
	}

    .unused_section : ALIGN (0x1000) { __kernel_end = . ; }
    /DISCARD/ : { *(.interp) }
}
