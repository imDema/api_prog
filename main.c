#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "commands.h"

#define MAXLEN 128

int main(int argc, char* argv[])
{
    //INIT OPS
    char line[MAXLEN + 1];

    //START READING INPUTS
    while(strcmp(fgets(line, MAXLEN, stdin), "end\n"))
    {
        char* opcode = strtok(line, " \"");

        if(strcmp(opcode, "addent")) //ADDENT
        {
            char* id_ent = strtok(NULL, " \"");
            addent(id_ent);
        }
        else if (strcmp(opcode, "delent")) //DELENT
        {
            char* id_ent = strtok(NULL, " \"");
            delent(id_ent);
        }
        else if (strcmp(opcode, "addrel")) //ADDREL
        {
            char* id_orig = strtok(NULL, " \"");
            char* id_dest = strtok(NULL, " \"");
            char* id_rel = strtok(NULL, " \"");
            addrel(id_orig,id_dest,id_rel);
        }
        else if (strcmp(opcode, "delrel")) //DELREL
        {
            char* id_orig = strtok(NULL, " \"");
            char* id_dest = strtok(NULL, " \"");
            char* id_rel = strtok(NULL, " \"");
            delrel(id_orig,id_dest,id_rel);
        }
        else if (strcmp(opcode, "report")) //REPORT
        {
            report();
        }
        else
            exit(666);
    }

}