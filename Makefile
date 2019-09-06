# Cleaned and fixed by StackOverflow user Michael Petch
C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c fs/*.c)
NASM_SOURCES = $(wildcard kernel/*.asm drivers/*.asm cpu/*.asm libc/*.asm fs/*.asm)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h fs/*.h)
S_SOURCES = $(wildcard kernel/*.s drivers/*.s cpu/*.s libc/*.s fs/*.s *.s)
 
# Nice syntax for file extension replacement
OBJ = ${C_SOURCES:.c=.o}  ${NASM_SOURCES:.asm=.o} ${S_SOURCES:.s=.o}
 
# Change this if your cross-compiler is somewhere else
CC = /usr/bin/i686-elf-gcc
GDB = gdb
# -g: Use debugging symbols in gcc
CFLAGS = -g #-m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra
 
# First rule is run by default
myos.iso: kernel.elf
	grub-file --is-x86-multiboot kernel.elf
	mkdir -p isodir/boot/grub
	cp kernel.elf isodir/boot/os-image.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o DripOS.iso isodir
 
# '--oformat binary' deletes all symbols as a collateral, so we don't need
# to 'strip' them manually on this case
kernel.bin: kernel.elf
	objcopy -O binary $^ $@
 
# Used for debugging purposes
kernel.elf: ${OBJ}
	i686-elf-ld -melf_i386 -o $@ -T linker.ld $^
 
run: myos.iso
	qemu-system-x86_64 -soundhw pcspk -device isa-debug-exit,iobase=0xf4,iosize=0x04 -boot menu=on -cdrom DripOS.iso -hda dripdisk.img
 
iso: myos.iso
	cp myos.iso doneiso/
# Open the connection to qemu and load our kernel-object file with symbols
debug: myos.iso
	qemu-system-x86_64 -soundhw pcspk -device isa-debug-exit,iobase=0xf4,iosize=0x04 -s -S -boot menu=on -cdrom DripOS.iso -hda dripdisk.img &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"
 
# Generic rules for wildcards
# To make an object, always compile from its .c $< $@
%.o: %.c ${HEADERS}
	i686-elf-gcc -O2 -g -MD -c $< -o $@ -std=gnu11
 
%.o: %.s
	i686-elf-gcc -O2 -g -MD -c $< -o $@
 
%.o: %.asm
	nasm -g -f elf32 -F dwarf -o $@ $<
 
clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf *.iso
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o
