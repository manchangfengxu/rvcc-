CFLAGS=-std=c11 -g -fno-common
CC=clang
file = main
rvcc:$(file).o
	$(CC) -o rvcc $(CFLAGS) $(file).o

test:rvcc
	./test.sh

clean:
	rm -f rvcc *.o *.s tmp* a.out

 .PHONY: test clean