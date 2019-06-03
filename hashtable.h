#include<stdlib.h>
#include<stdio.h>
#include "structures.h"

#define HASH_A 31

int hashchar(char c)
{
    if(c >= 'A' && c <= 'Z')
        return c + OFFSET_UPPER;
    
    else if (c >= 'a' && c <= 'z')
        return c + OFFSET_LOWER;

    else if(c >= '0' && c <= '9')
        return c + OFFSET_NUMBER;
    
    else if(c == '_')
        return UNDERSCORE;
    
    else 
        return DASH;
}

uint hash(char* word)
{
    uint h = 0;
    for(int i = 0; word[i]; i++)
    {
        h = (h*HASH_A) + word[i]; //ALLOW OVERFLOW
    }
    return h;
}

struct hashtable
{
    void** bucket;
    int count;
    int size;
};
typedef struct hashtable* Hashtable;

struct htelement
{

};

Hashtable new_hashtable(int size)
{
    Hashtable ht = (Hashtable) malloc(sizeof(struct hashtable));
    ht->size=size;
    ht->bucket = (void**) calloc(sizeof(void*),size);
    ht->count = 0;
    return ht;
}

void ht_insert(Hashtable ht, void* element, uint (*hashfun)(void*))
{
    uint hash = hashfun(element);
    int index = hash % ht->size;
    
}