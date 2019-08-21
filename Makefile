default: build

CC = gcc
CFLAGS = -std=gnu11 -Wall -pipe
MAIN = main.c

build:
	$(CC) $(CFLAGS) -O2 -s -o main $(MAIN)

debug:
	$(CC) $(CFLAGS) -Wuninitialized -Werror -pedantic -g3 -o main $(MAIN)

debug-optimized:
	$(CC) $(CFLAGS) -O2 -Wuninitialized -Werror -pedantic -g3 -o main $(MAIN)

test:
	$(CC) $(CFLAGS) -O2 -o tests/testhash.out tests/testhash.c