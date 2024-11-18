# Compiler and Assembler Configuration
CC = gcc
CC_ARGS = -finput-charset=UTF-8 -fexec-charset=UTF-8 -I ../include/kernel/ \
           -fno-stack-protector -m32 -mgeneral-regs-only -nostdlib -O0 -c

ASMC = nasm
ASMC_ARGS = -felf32

ARCH = i586

# Source and Object Directories
SRC = src/
OBJ = obj/
RAMFS = initrd/

# File Discovery
ASM_FILES_ARCH := $(addprefix ../,$(shell find $(SRC)kernel/arch/ -name "*.asm"))
C_FILES := $(addprefix ../,$(shell find $(SRC)kernel/ -name "*.c"))

# ISO Configuration
ISO_NAME = Unix-P6
ISO_VER = 0.0.1a
ISO_DIR = iso/

# Output Log
OUTPUTLOG = /dev/pts/0

# Build Targets
.PHONY: build run init clear ramfs

build:
	#==[ NASM  ]==#
	@cd $(OBJ); \
	for element in $(notdir $(ASM_FILES_ARCH)); do \
		$(ASMC) $(ASMC_ARGS) ../src/kernel/arch/$$element -o $$element.o; \
		echo \~ Building asm object $$element; \
	done
	
	#==[ GCC   ]==#
	@cd $(OBJ); \
	for element in $(C_FILES); do \
		filename=$$(basename $$element); \
		$(CC) $(CC_ARGS) $$element -o $$filename.o; \
		echo \~ Building C object $$filename; \
	done
	
	#==[ LD    ]==#
	ld -m elf_i386 -T link.ld -o $(ISO_DIR)boot/kernel.elf $(shell find $(OBJ) -name "*.o")
	
	#==[ RAMFS ]==#
	@tar -cf $(ISO_DIR)boot/initrd.tar $(RAMFS)
	
	#==[ GRUB  ]==#
	@grub-mkrescue -o $(ISO_NAME)-$(ISO_VER).iso $(ISO_DIR)

run:
	@qemu-system-x86_64 -cpu pentium3 -serial file:serial.log \
	    -accel kvm -m 512m -boot d \
	    -cdrom $(ISO_NAME)-$(ISO_VER).iso \
	    -netdev socket,id=n0,listen=:2030 \
	    -device rtl8139,netdev=n0,mac=11:11:11:11:11:11 

init:
	@mkdir -p $(OBJ)

clear:
	rm -rf $(OBJ)/*
	rm -f $(ISO_DIR)boot/kernel.elf

ramfs:
	@tar -cvf iso/sys/initramfs.tar -C initramfs .
