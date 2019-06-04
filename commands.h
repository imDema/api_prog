#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashtable.h"

void addent(char* id_ent)
{
    //Calculate hash
    //Search entry in entity ht
        //Add if it doesn't exist
}

void delent(char* id_ent)
{
    //Go to the entry in the entity hashtable
    //Iterate over links
    //Free link entries
    //Update top list
}

void addrel(char* id_orig, char* id_dest, char* id_rel)
{
    //Check relation list in rel_ht
        //If it doesn't create a new entry and increment nonce
    //Calculate combined hash
    //Search entry in link hashtable
        //Create entry if it does not exist
    //Set active relation arraylist to proper value (update count)
    //Add link pointer to both entities entries list
    //TODO: Update max lists 
}

void delrel(char* id_orig, char* id_dest, char* id_rel)
{
    //Calculate combined hash
    //Search entry in link hashtable
        //Skip if it does not exist
    //Update arraylist entry if needed
    //Reduce count if arraylist entry becomes 0
    //If count is 0 delete link entry
    //TODO: Update max lists
}

void report()
{
    //Sort relation entries
        //qsort()
    //If count == 0 skip
    //Query max lists to get top element
    //Print
}
