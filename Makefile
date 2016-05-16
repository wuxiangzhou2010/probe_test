CONFIG_MODULE_SIG=n
#obj-m += kprobe_example.o jprobe_example.o
#obj-m += kretprobe_example.o
#obj-m += kobj.o
#obj-m += class.o
obj-m += wuxiang.o


all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

