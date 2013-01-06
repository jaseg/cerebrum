
all: main

main: autocode.c config.c uart.c ../common/main.c ../common/comm.c
	@gcc -std=gnu99 -D__TEST__ -o main -I ../common -I. $^
clean:
	rm -f main || true
program:
