default: build

build:
	gcc main.c -O2 -static -pipe -s

debug:
	gcc main.c -pipe -Wuninitialized -Wall -Werror -pedantic -g3