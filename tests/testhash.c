#include <stdio.h>
#include <stdlib.h>
#include "../structures.h"


int main(void) {
	// your code goes here
	char c = '_';
    char str[4];
    int i = 0;

	while(fgets(str, 4, stdin))
	{
		c = str[0];
		printf("%c:%d\n", c, hash(c) - i++);
	}
	return 0;
}
