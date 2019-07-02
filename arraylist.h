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
    if(deleted && arl->array[index] == 0)
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
