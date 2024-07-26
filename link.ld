OUTPUT_FORMAT(elf32-i386)
ENTRY(_start)

SECTIONS
 {
   . = 0x1000;
   .text : { *(.text) }
   .data : { *(.data) }
   .bss  : { *(.bss)  }
   .unused_section : ALIGN (0x1000) 
   { __kernel_end = . ; }

 }
