#include<stdlib.h>
#define RELARRAY_DEFAULTSIZE 4

typedef u_int8_t byte;
struct _relarray
{
    byte* array;
    int size;
    int count;
};
typedef struct _relarray* relarray;


void relarray_add(relarray arl, int index, byte mask)
{
    if(index >= arl->size)
    {
        while(arl->size < index)
            arl->size <<= 1;
        arl->array = (byte*)realloc(arl->array, arl->size * sizeof(byte));
    }
    arl->array[index] |= mask;
}

void relarray_remove(relarray arl, int index, byte mask)
{
    if(index >= arl->size) return;
    arl->array[index] &= !mask;
    if(arl->array[index] == 0b0)
    {
        arl->count--;
    }
}

void relarray_free(relarray arl)
{
    free(arl->array);
    free(arl);
}

relarray new_relarray()
{
    relarray arl = (relarray) malloc(sizeof(struct _relarray));
    arl->array = (byte*) calloc(RELARRAY_DEFAULTSIZE,sizeof(byte));
    arl->size = RELARRAY_DEFAULTSIZE;
    arl->count = 0;
    return arl;
}