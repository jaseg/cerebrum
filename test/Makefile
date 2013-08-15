
all: main

main: autocode.c config.c uart.c main.c ../common/comm.c
	@gcc -std=gnu99 -D__TEST__ -DCONFIG_ADDRESS=${CONFIG_ADDRESS} -o main -I ../common -I. $^
clean:
	@rm -f main || true
program:
