default: build

build:
	gcc main.c -O2 -static -pipe -s

debug:
	gcc main.c -pipe -Wuninitialized -Wall -Werror -pedantic -g3

test:
	gcc tests/testhash.c -O2 -static -pipe -o tests/testhash.out