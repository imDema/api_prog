default: build

build:
	gcc main.c -O2

debug:
	gcc main.c -Wuninitialized -Wall -Werror -pedantic -g3