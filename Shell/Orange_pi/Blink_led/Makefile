EXTRA_CFLAGS=-Wall

obj-m = blinkled.o

KDIR = /lib/modules/`uname -r`/build

all:	
	$(MAKE) -C $(KDIR) M=`pwd` modules
clean:
	$(MAKE) -C $(KDIR) M=`pwd` clean
