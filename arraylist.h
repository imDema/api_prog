#include<stdlib.h>
#define ARRAYLIST_DEFAULTSIZE 4

typedef struct _arraylist
{
    char** array;
    int size = ARRAYLIST_DEFAULTSIZE;
    int count = 0;
} Arraylist;


void arraylist_add(Arraylist arraylist, char* item)
{
    if(arraylist.count == arraylist.size)
    {
        arraylist.size *= 2;
        arraylist.array = (char**)realloc(arraylist.array, arraylist.size);
    }
    arraylist.array[arraylist.count++] = item;
}

void arraylist_remove(Arraylist arraylist, char* item)
{
        
}

Arraylist new_arraylist()
{
    Arraylist ar;
    ar.array = (char**) malloc(sizeof(char*)*ARRAYLIST_DEFAULTSIZE);
    ar.size = ARRAYLIST_DEFAULTSIZE;
    ar.count = 0;
    return ar;
}