
all: comm-test

comm-test:
	@gcc -std=gnu99 -D__TEST__ -o comm-test comm.c main.c uart.c autocode.c config.c

clean:
	rm comm-test

