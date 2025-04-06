CFILES = $(wildcard *.c drivers/*.c)
OFILES = $(CFILES:.c=.o)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles -mstrict-align -Iinclude

all: clean kernel8.img

boot.o: boot.S
	aarch64-elf-gcc $(GCCFLAGS) -c boot.S -o boot.o

%.o: %.c
	aarch64-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: boot.o $(OFILES)
	aarch64-elf-ld -nostdlib boot.o $(OFILES) -T linker.ld -o kernel8.elf
	aarch64-elf-objcopy -O binary kernel8.elf kernel8.img

run:
	qemu-system-aarch64 \
    -M raspi4b \
	-cpu cortex-a72 \
	-accel tcg \
    -kernel kernel8.img \
    -m 2048 \
    -serial stdio \
    -display cocoa

clean:
	rm -rf kernel8.elf *.o *.img > /dev/null 2> /dev/null || true
	rm -rf drivers/*.o