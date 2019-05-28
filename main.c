#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXLEN 128

int main(int argc, char* argv[])
{
    //INIT OPS
    char line[MAXLEN + 1];

    //START READING INPUTS
    while(strcmp(fgets(line, MAXLEN, stdin), "end\n"))
    {
        char* opcode = strtok(line, " \"");

        if(strcmp(opcode, "addent"))
        {
            
        }
        else if (strcmp(opcode, "delent"))
        {
            
        }
        else if (strcmp(opcode, "addrel"))
        {
            
        }
        else if (strcmp(opcode, "delrel"))
        {
            
        }
        else if (strcmp(opcode, "report"))
        {
            
        }
        else
            exit(666);
    }

}