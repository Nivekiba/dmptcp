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
	gcc -o server.o -g Server_listening.c
	gcc -o client.o -g Client_connect.c

serv:
	make clean && make exes && ./server.o

client:
	make clean && make exes && ./client.o

debug_server:
	make clean && make exes && gdb ./server.o

debug_client:
	make clean && make exes && gdb ./client.o