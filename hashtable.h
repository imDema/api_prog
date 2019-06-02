#include<stdlib.h>
#include<stdio.h>
#include "structures.h"

uint hash(char* word)
{
    uint h0 = 1;
    for(int i = 0; word[i]; i++)
    {
        if(i%2)
        {
            h0 += hashchar(word[i]) * (i+1)*(i+1);
        }
        else
        {
            h0 ^= hashchar(word[i]) << (i*2)%16;
        }
    }
    return h0;
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