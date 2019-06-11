default: build

CC = gcc
CFLAGS = -std=gnu11 -Wall -static -pipe -s
MAIN = main.c

build:
	$(CC) $(MAIN) $(CFLAGS) -O2 -o main

debug:
	$(CC) $(MAIN) $(CFLAGS) -Wuninitialized -Werror -pedantic -g3 -o main

test:
	$(CC) tests/testhash.c $(CFLAGS) -O2 -o tests/testhash.out