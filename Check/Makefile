obj-m = BM_Driver.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm a.out || rm tester
	rm tester

test:
	gcc tester.c -o tester && ./tester
	
mknd:
	mknod /dev/BMDriver c 45 1
	
debg:
	dmesg | tail -100