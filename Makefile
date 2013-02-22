includes = util.inc multiboot.inc io.inc

all: kernel.bin

kernel.lst kernel.bin kernel.fas: kernel.s $(includes)
	./fasm/fasm -s kernel.fas kernel.s kernel.bin
	./fasm/listing -b 8 kernel.fas kernel.lst

run: kernel.bin
	qemu -kernel kernel.bin -debugcon vc
