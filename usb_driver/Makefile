CONFIG_MODULE_SIG=n

obj-m += usb.o

KDIR = /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd` modules
clean:
	make -C $(KDIR) M=`pwd` clean
