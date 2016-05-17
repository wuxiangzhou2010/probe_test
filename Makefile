CONFIG_DEBUG_KMEMLEAK=y
#ONFIG_MODULE_SIG=n
ONFIG_DEBUG_KMEMLEAK_EARLY_LOG_SIZE=400
#obj-m += kprobe_example.o jprobe_example.o
#obj-m += kretprobe_example.o
#obj-m += kobj.o
#obj-m += class.o
obj-m += wuxiang.o
#obj-m += kmemcheck.o
#obj-m += kmemleak-test.o
obj-m += leak.o
#obj-m += sys.o
EXTRA_CFLAGS = -O $(include_dirs) #-std=c99 
include_dirs = -I/usr/src/linux-headers-4.2.0-27/include/linux/

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

