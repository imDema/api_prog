#include <stdio.h>
#include <stdlib.h>
#include "../hashtable.h"


int main(int argc, char* argv[]) {
	// your code goes here
    char str[128];
	if (argc != 2)
	{
		fprintf(stderr, "SPECIFY MODULE IN argv\n");
		exit(66);
	}
	int mod = atoi(argv[1]);

	FILE* outptr = fopen("tests/hashed.txt","w+");
	FILE* inptr = fopen("tests/words.txt","r");

	while(fgets(str, 128, inptr))
	{
		fprintf(outptr,"%08x\n", hash(str) % mod);
	}

	fclose(outptr);
	fclose(inptr);
	return 0;
}
