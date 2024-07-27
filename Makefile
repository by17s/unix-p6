CC=gcc
CC_ARGS=-finput-charset=UTF-8 -fexec-charset=UTF-8 -I ../include/kernel/ -fno-stack-protector -m32 -mgeneral-regs-only -nostdlib -O0 -c

ASMC=nasm
ASMC_ARGS=-felf32

ARCH=i586

SRC=src/
OBJ=obj/

ASM_FILES_ARCH := $(shell find $(SRC)kernel/arch/ -name "*.asm")
C_FILES := $(addprefix ../,$(shell find $(SRC)kernel/ -name "*.c"))
#C_FILES_KDRIVERS := $(addprefix ../,$(shell find $(SRC)kdrivers/ -name "*.c"))

ISO_NAME=Unix-P6
ISO_VER=0.0.1a
ISO_DIR=iso/

OUTPUTLOG=/dev/pts/0

build:
	#==[ NASM ]==#
	@cd obj;$(ASMC) $(ASMC_ARGS) ../$(ASM_FILES_ARCH) -o aarch.o
	#==[ GCC  ]==#
	@cd obj;$(CC) $(CC_ARGS) $(C_FILES)
	#==[ LD   ]==#
	@ld -m elf_i386 -T link.ld -o $(ISO_DIR)boot/kernel.elf $(shell find obj -name "*.o")
	#==[ GRUB ]==#
	@grub-mkrescue -o $(ISO_NAME)-$(ISO_VER).iso $(ISO_DIR)

run:
	@qemu-system-x86_64 -cpu pentium3 -serial file:serial.log -accel kvm -m 512m -boot d -cdrom $(ISO_NAME)-$(ISO_VER).iso -netdev socket,id=n0,listen=:2030 -device rtl8139,netdev=n0,mac=11:11:11:11:11:11 

clear:
	rm obj/*

ramfs:
	tar -cvf iso/sys/initramfs.tar -C initramfs .