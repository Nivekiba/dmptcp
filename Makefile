obj-m := LKM.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
	rm -r -f *.mod.c .*.cmd *.symvers *.o

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean

install:
	insmod $(shell pwd)/LKM.ko

remove:
	rmmod LKM

exes:
	gcc -o server.o Server_listening.c
	gcc -o client.o Client_connect.c