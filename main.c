#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "arraylist.h"
#include "hashtable.h"
#include "structures.h"
#include "commands.h"

#define MAXLEN 128
#define PRIME_SMALL 31
#define PRIME_K 1021
#define PRIME_M 1048573

typedef unsigned int uint;



int main(int argc, char* argv[])
{
    //INIT OPS
    char line[MAXLEN + 1];

    hashtable link_ht, ent_ht, rel_ht;
    link_ht = new_hashtable(PRIME_SMALL);
    ent_ht = new_hashtable(PRIME_SMALL);
    rel_ht = new_hashtable(PRIME_SMALL);

    //START READING INPUTS
    while(strcmp(fgets(line, MAXLEN, stdin), "end\n"))
    {
        char* opcode = strtok(line, " \"");

        if(!strcmp(opcode, "addent")) //ADDENT
        {
            char* id_ent = strtok(NULL, " \"");
            addent(ent_ht, id_ent);
        }
        else if (!strcmp(opcode, "delent")) //DELENT
        {
            char* id_ent = strtok(NULL, " \"");
            delent(id_ent);
        }
        else if (!strcmp(opcode, "addrel")) //ADDREL
        {
            char* id_orig = strtok(NULL, " \"");
            char* id_dest = strtok(NULL, " \"");
            char* id_rel = strtok(NULL, " \"");
            addrel(ent_ht, link_ht, rel_ht, id_orig,id_dest,id_rel);
        }
        else if (!strcmp(opcode, "delrel")) //DELREL
        {
            char* id_orig = strtok(NULL, " \"");
            char* id_dest = strtok(NULL, " \"");
            char* id_rel = strtok(NULL, " \"");
            delrel(id_orig,id_dest,id_rel);
        }
        else if (!strcmp(opcode, "report")) //REPORT
        {
            report();
        }
        else
            exit(666);
    }
    //FREE OPS
    ht_free(link_ht, ht_link_free);
    ht_free(ent_ht, ht_ent_free);
    ht_free(rel_ht, ht_ent_free);
}