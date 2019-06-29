#define MAXLEN 128
typedef unsigned int uint;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "arraylist.h"
#include "hashtable.h"
//#include "toplist.h"
#include "structures.h"
#include "commands.h"

int main(int argc, char* argv[])
{
    //INIT OPS
    char line[MAXLEN + 1];

    rel_db relations = new_rel_db();
    direct_ht ht = new_direct_ht(DEFAULT_DIRECT_HT_SIZE);

    FILE* inptr = stdin;
    inptr = fopen("tests/test.in","r"); //FOR DEBUGGING

    //int i = 0;

    //START READING INPUTS
    while(strcmp(fgets(line, MAXLEN, inptr), "end\n"))
    {
        //fputs(line, stdout); //DEBUGGING ONLY
        //if(strstr(line, "Problematic_entry"))
            //line[0] = line[0];

        char* opcode = strtok(line, " \"\n");

        if(!strcmp(opcode, "addent")) //ADDENT
        {
            char* id_ent = strtok(NULL, " \"\n");
            addent(ht,id_ent);
        }
        else if (!strcmp(opcode, "delent")) //DELENT
        {
            char* id_ent = strtok(NULL, " \"\n");
            delent(ht, relations, id_ent);
        }
        else if (!strcmp(opcode, "addrel")) //ADDREL
        {
            char* id_orig = strtok(NULL, " \"\n");
            char* id_dest = strtok(NULL, " \"\n");
            char* id_rel = strtok(NULL, " \"\n");
            addrel(ht, relations, id_orig, id_dest, id_rel);
        }
        else if (!strcmp(opcode, "delrel")) //DELREL
        {
            char* id_orig = strtok(NULL, " \"\n");
            char* id_dest = strtok(NULL, " \"\n");
            char* id_rel = strtok(NULL, " \"\n");
            delrel(ht, relations, id_orig, id_dest, id_rel);
        }
        else if (!strcmp(opcode, "report")) //REPORT
        {
            report(ht, relations);
        }
        else if(!strcmp(opcode, "debug")) //DEBUG
        {
            line[0] = line[0];
        }
        else
            fprintf(stderr, "Unrecognized command!\n");
    }

    if(inptr != stdin) fclose(inptr); //FOR DEBUGGING
    //FREE OPS
    free_entities(ht);
    rel_db_free(relations);
}