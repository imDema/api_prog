#include<stdlib.h>
#include<stdio.h>
#include "arraylist.h"

#define HASH_A 31

#define OFFSET_UPPER -65 // -'A';
#define OFFSET_NUMBER -22 // -'0' + 'z'-'a' + 1
#define OFFSET_LOWER -61 //- 'a' + 'Z' - 'A' + '9' - '0' + 2;
#define UNDERSCORE 62
#define DASH 63

typedef unsigned int uint;

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

struct _hashtable
{
    void** buckets;
    int count;
    int size;
};
typedef struct _hashtable* hashtable;

hashtable new_hashtable(int size)
{
    hashtable ht = (hashtable) malloc(sizeof(struct _hashtable));
    ht->size=size;
    ht->buckets = (void**) calloc(sizeof(void*),size);
    ht->count = 0;
    return ht;
}

void ht_insert(hashtable ht, void* element, uint (*hashfun)(void*), void (*chain)(void*,void*))
{
    uint hash = hashfun(element);
    int index = hash % ht->size;
    if(ht->buckets[index] == NULL)
    {
        ht->buckets[index] = element;
    }
    else
    {
        chain(ht->buckets[index], element);
    }
    ht->count++;
}

void ht_free(hashtable ht, void (*entry_free)(void*))
{
    for(int i = 0; ht->count > 0; i++) //IF STUFF CRASHES IT'S BECAUSE ht->count is wrong
    {
        // {
        //     if(i >= ht->size)
        //     fprintf(stderr, "Error freeing hashtable\nht->size=%d\nht->count=%d\ni=%d", ht->size, ht->count, i);
        // }
        if(ht->buckets[i] != NULL)
        {
            entry_free(ht->buckets[i]);
            ht->count--;
        }
    }
}

void ht_link_free(void* entry);
void ht_rel_free(void* entry);
void ht_ent_free(void* entry);