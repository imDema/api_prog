#define ARRAYLIST_DEFAULTSIZE 1

typedef unsigned char byte;
struct _relarray
{
    uint* array;
    int size;
    int count;
};
typedef struct _relarray* relarray;

const int wsize = 8 * sizeof(uint);

int relarray_add(relarray arl, int index)
{
    if(index >= arl->size)
    {
        while(arl->size < index)
            arl->size <<= 1;
        arl->array = realloc(arl->array, arl->size / (wsize) * sizeof(byte));
    }
    int i = index / wsize;
    int j = index % wsize;
    int newcreated = !((arl->array[index] >> j) & 0x1);
    arl->array[i] |= (0x1 << j);
    return newcreated;
}

int relarray_remove(relarray arl, int index)
{
    if(index >= arl->size) return 0;
    
    int i = index / wsize;
    int j = index % wsize;

    int deleted = (arl->array[index] >> j) & 0x1;
    if(deleted)
    {
        arl->array[i] &= ~(0x1 << j);
        arl->count--;
    }
    return deleted;
}

int* relarray_get_active(relarray arl)
{
    int* result = malloc(arl -> count * sizeof(uint));
    int i = 0;
    int j = 0;
    int k = 0;
    uint slider;
    while(k < arl->count)
    {
        if(j == 0)
            slider = arl->array[i++];
        if(slider & 0x1)
        {
            result[k++] = i * wsize + j;    //Possible segfault if done wrong
        }
        slider >>= 1;
        j = (j + 1) % wsize;
    }
    return result;
}

void relarray_free(relarray arl)
{
    free(arl->array);
    free(arl);
}

relarray new_relarray()
{
    relarray arl = malloc(sizeof(struct _relarray));
    arl->array = calloc(ARRAYLIST_DEFAULTSIZE,sizeof(uint));
    arl->size = ARRAYLIST_DEFAULTSIZE * wsize;
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
    if(acl->array == NULL)
    {
        acl->array = (int*) calloc(ARRAYLIST_DEFAULTSIZE,sizeof(int));
        acl->size = ARRAYLIST_DEFAULTSIZE;
        acl->count = 0;
    }
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