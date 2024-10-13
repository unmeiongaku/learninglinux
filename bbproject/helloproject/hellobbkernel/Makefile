obj-m += main.o

CROSS=/home/desmtiny/workspace/BBboard/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KERNEL=/home/desmtiny/workspace/BBboard/kernelbuildscripts/KERNEL/

all:
	make ARCH=arm CROSS_COMPILE=${CROSS} -C ${KERNEL} M=$(PWD) modules
clean:
	make -C ${KERNEL} M=$(PWD) clean
copy:
	cp main.ko /media/desmtiny/rootfs/home/debian/workingspace/hellopj/