#define ARRAYLIST_DEFAULTSIZE 1
#define FROM_FIRST 0x1
#define FROM_SECOND 0x2

const int wsize = 8 * sizeof(uint);

typedef unsigned char byte;
struct _relarray
{
    byte* array;
    int size;
    int count;
};
typedef struct _relarray* relarray;

struct _countarray
{
    int* array;
    int size;
    int count;
};
typedef struct _countarray countarray;

typedef struct _toparray
{
    char* id_rel;
    int value;
    char** array;
    int count;
    int size;
} toparray;

int relarray_add(relarray arl, int index, int direction)
{
    byte mask = direction <= 0 ? FROM_FIRST : FROM_SECOND;
    if(index >= arl->size)
    {
        int s0 = arl->size;
        while(index >= arl->size)
            arl->size *= 2;
        arl->array = (byte*)realloc(arl->array, arl->size * sizeof(byte));
        for(int i = s0; i < arl->size; i++)
            arl->array[i] = 0;
    }
    int newcreated = !(arl->array[index] & mask);
    if(newcreated)
    {
        arl->count++;
    }

    arl->array[index] |= mask;
    return newcreated;
}

int relarray_remove(relarray arl, int index, int direction)
{
    if(index >= arl->size) return 0;
    byte mask = direction <= 0 ? FROM_FIRST : FROM_SECOND;

    int deleted = arl->array[index] & mask;
    arl->array[index] &= ~mask;
    if(arl->array[index] == 0)
        arl->count--;
    return deleted;
}

void relarray_free(relarray arl)
{
    if(arl == NULL) return;
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

int countarray_increase(countarray* acl, int index)
{
    if(acl->array == NULL)
    {
        acl->array = (int*) calloc(ARRAYLIST_DEFAULTSIZE,sizeof(int));
        acl->size = ARRAYLIST_DEFAULTSIZE;
        acl->count = 0;
    }
    if(index >= acl->size)
    {
        int s0 = acl->size;
        while(index >= acl->size)
            acl->size *= 2;
        acl->array = (int*)realloc(acl->array, acl->size * sizeof(int));
        for(int i = s0; i < acl->size; i++)
            acl->array[i] = 0;
    }
    //If it's the first for this kind of relation increase count of active relations
    if(acl->array[index] == 0) acl->count++;
    return ++acl->array[index];
}

int countarray_reduce(countarray* acl, int index)
{
    if(index >= acl->size) return 0;
    if(acl->array[index]-1 == 0)
        acl->count--;
    
    return --acl->array[index];
}

void countarray_free(countarray* acl)
{
    free(acl->array);
    free(acl);
}

void arralylist_push(toparray* tarr, void* item)
{
    if(tarr->size == 0)
    {
        tarr->array = calloc(ARRAYLIST_DEFAULTSIZE, sizeof(char*));
        tarr->size = ARRAYLIST_DEFAULTSIZE;
        tarr->count = 0;
    }
    if(tarr->size == tarr->count)
    {
        tarr->size *= 2;
        tarr->array = realloc(tarr->array, tarr->size * sizeof(char*));
    }
    tarr->array[tarr->count] = item;
    tarr->count++;
}