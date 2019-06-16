#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "arraylist.h"
#include "hashtable.h"
//#include "toplist.h"
#include "structures.h"
#include "commands.h"

#define MAXLEN 128
#define PRIME_DEBUG 17
#define PRIME_SMALL 31
#define PRIME_K 1021
#define PRIME_M 1048573

typedef unsigned int uint;



int main(int argc, char* argv[])
{
    //INIT OPS
    char line[MAXLEN + 1];

    rel_db relations = new_rel_db();
    direct_ht ht = new_direct_ht(DEFAULT_DIRECT_HT_SIZE);

    //START READING INPUTS
    while(strcmp(fgets(line, MAXLEN, stdin), "end\n"))
    {
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
            //report();
        }
        else
            fprintf(stderr, "Unrecognized command!\n");
    }
    //FREE OPS
    ht_free(ht);
    rel_db_free(relations);
}