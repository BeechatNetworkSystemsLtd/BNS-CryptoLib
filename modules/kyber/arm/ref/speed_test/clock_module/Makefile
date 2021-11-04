obj-m+=clock_enabler.o

all:
	make -C /lib/modules/$(shell uname -r)/build oldconfig
	make -C /lib/modules/$(shell uname -r)/build prepare
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
