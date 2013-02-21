includes = util.inc multiboot.inc io.inc

kernel.bin kernel.fas: kernel.s $(includes)
	./fasm/fasm -s kernel.fas kernel.s kernel.bin

#kernel.lst: kernel.fas
#	./fasm/listing kernel.fas kernel.lst

run: kernel.bin
	qemu -kernel kernel.bin -debugcon vc
