obj-m += drvled.o
obj-m += drvled_gpio.o
 
ifdef ARCH
  #You can update your Beaglebone path here.
  KDIR = /home/BBG/lib/modules/5.10.65/build
else
  #KDIR = /lib/modules/$(shell uname -r)/build
  KDIR = /lib/modules/5.4.0-169-generic/build
endif
 
all:
	make -C $(KDIR)  M=$(shell pwd) modules
 
clean:
	make -C $(KDIR)  M=$(shell pwd) clean

