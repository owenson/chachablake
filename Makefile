CC=gcc -std=c99 -I include

main: blake2b.o chacha.o main.o
	$(CC) -o test main.o blake2b.o chacha.o

%.o: %.c
	$(CC) -c -o $@ $<

clean:
	rm *.o test
