#define ARRAYLIST_DEFAULTSIZE 2

typedef unsigned char byte;
struct _relarray
{
    byte* array;
    int size;
    int count;
};
typedef struct _relarray* relarray;


int relarray_add(relarray arl, int index, byte mask)
{
    if(index >= arl->size)
    {
        while(arl->size < index)
            arl->size <<= 1;
        arl->array = (byte*)realloc(arl->array, arl->size * sizeof(byte));
    }
    int newcreated = !(arl->array[index] & mask);
    arl->array[index] |= mask;
    return newcreated;
}

int relarray_remove(relarray arl, int index, byte mask)
{
    if(index >= arl->size) return;

    int deleted = arl->array[index] & mask;
    arl->array[index] &= ~mask;
    if(arl->array[index] == 0)
        arl->count--;
    return deleted;
}

void relarray_free(relarray arl)
{
    free(arl->array);
    free(arl);
}

relarray new_relarray()
{
    relarray arl = malloc(sizeof(struct _relarray));
    arl->array = (byte*) calloc(ARRAYLIST_DEFAULTSIZE,sizeof(byte));
    arl->size = ARRAYLIST_DEFAULTSIZE;
    arl->count = 0;
    return arl;
}

struct _countarray
{
    int* array;
    int size;
    int count;
};
typedef struct _countarray * countarray;


int countarray_increase(countarray acl, int index)
{
    if(index >= acl->size)
    {
        while(acl->size < index)
            acl->size <<= 1;
        acl->array = (int*)realloc(acl->array, acl->size * sizeof(int));
    }
    //If it's the first for this kind of relation increase count of active relations
    if(acl->array[index] == 0) acl->count++;
    return ++acl->array[index];
}

int countarray_reduce(countarray acl, int index)
{
    if(index >= acl->size) return 0;
    if(acl->array[index]-1 == 0)
        acl->count--;
    
    return --acl->array[index];
}

void countarray_free(countarray acl)
{
    free(acl->array);
    free(acl);
}

countarray new_countarray()
{
    countarray acl = malloc(sizeof(struct _countarray));
    acl->array = (int*) calloc(ARRAYLIST_DEFAULTSIZE,sizeof(int));
    acl->size = ARRAYLIST_DEFAULTSIZE;
    acl->count = 0;
    return acl;
}