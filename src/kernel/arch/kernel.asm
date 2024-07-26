

bits 32

%define MULTIBOOT2_HEADER_MAGIC			0xe85250d6
%define MULTIBOOT_ARCHITECTURE_I386  0
%define MULTIBOOT_HEADER_TAG_OPTIONAL 1

%define MULTIBOOT_HEADER_TAG_END  0
%define MULTIBOOT_HEADER_TAG_INFORMATION_REQUEST  1
%define MULTIBOOT_HEADER_TAG_ADDRESS  2
%define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS  3
%define MULTIBOOT_HEADER_TAG_CONSOLE_FLAGS  4
%define MULTIBOOT_HEADER_TAG_FRAMEBUFFER  5
%define MULTIBOOT_HEADER_TAG_MODULE_ALIGN  6
%define MULTIBOOT_HEADER_TAG_EFI_BS        7
%define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI32  8
%define MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS_EFI64  9
%define MULTIBOOT_HEADER_TAG_RELOCATABLE  10

extern kmain
extern __kernel_end

global _start
align 8

section .mbheader

multiboot_header:
    multiboot_header.magic:         dd MULTIBOOT2_HEADER_MAGIC
    multiboot_header.arch:          dd MULTIBOOT_ARCHITECTURE_I386
    multiboot_header.size:          dd multiboot_header_end - multiboot_header
    multiboot_header.checksum:      dd -(MULTIBOOT2_HEADER_MAGIC + MULTIBOOT_ARCHITECTURE_I386 + (multiboot_header_end - multiboot_header))

align 8

entry_address_tag_start:        
    dw MULTIBOOT_HEADER_TAG_ENTRY_ADDRESS
    dw MULTIBOOT_HEADER_TAG_OPTIONAL
    dd entry_address_tag_end - entry_address_tag_start

    dd _start       ; entry addr
entry_address_tag_end:

align 8

framebuffer_tag_start:
    dw MULTIBOOT_HEADER_TAG_FRAMEBUFFER
    dw 0
    dd framebuffer_tag_end - framebuffer_tag_start
    
    dd 800
    dd 600
    dd 0
framebuffer_tag_end:

align 8

    dw MULTIBOOT_HEADER_TAG_END
    dw 0
    dd 8
multiboot_header_end:

section .text

_start:
    cli

    mov esp, os_stack_top

    push ebx    ; bootloader multiboot header structure
    push eax    ; multiboot magic

    xor ebp, ebp ; mark end of stack trace

    call kmain

    cli

halt:
    hlt
    jmp halt

section .bss
align 16
    resb 0x2000
    os_stack_top:
    __bss_end:

section .data
