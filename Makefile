CC=gcc
CC_ARGS=-finput-charset=UTF-8 -fexec-charset=UTF-8 -I ../include/kernel/ -fno-stack-protector -m32 -mgeneral-regs-only -nostdlib -O0

ASMC=nasm
ASMC_ARGS=-felf32

GRUB_MKRESC=grub-mkrescue
ifeq (, $(shell type -t $(GRUB_MKRESC)))
GRUB_MKRESC=grub2-mkrescue
endif
ifeq (, $(shell type -t $(GRUB_MKRESC)))
$(error "grub-mkrescue not found")
endif

ARCH=i586

SRC=src/
OBJ=obj/

ASM_FILES_ARCH := $(shell find $(SRC)kernel/arch/ -name "*.asm")
C_FILES := $(addprefix ../,$(shell find $(SRC)kernel/ -name "*.c"))

ISO_NAME=Unix-P6
ISO_VER=0.0.1a
ISO_DIR=iso/

build:
	#==[ NASM ]==#
	@mkdir -p obj
	@mkdir -p iso/boot
	@cd obj; $(ASMC) $(ASMC_ARGS) ../$(ASM_FILES_ARCH) -o aarch.o; cd ..
	#==[ GCC  ]==#
	@cd obj; $(CC) $(CC_ARGS) -c $(C_FILES); cd ..
	#==[ LD   ]==#
	@ld -m elf_i386 -T link.ld --no-warn-rwx-segment -o $(ISO_DIR)boot/kernel.elf $(shell find obj -name "*.o")
	#==[ GRUB ]==#
	@$(GRUB_MKRESC) -o $(ISO_NAME)-$(ISO_VER).iso $(ISO_DIR)

run:
	@qemu-system-x86_64 -serial file:serial.log -accel kvm -m 512m -boot d -cdrom $(ISO_NAME)-$(ISO_VER).iso -netdev socket,id=n0,listen=:2030 -device rtl8139,netdev=n0,mac=11:11:11:11:11:11 

clean:
	rm obj/*
