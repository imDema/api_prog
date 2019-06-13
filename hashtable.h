#define HASH_A 33

#define OFFSET_UPPER -65 // -'A';
#define OFFSET_NUMBER -22 // -'0' + 'z'-'a' + 1
#define OFFSET_LOWER -61 //- 'a' + 'Z' - 'A' + '9' - '0' + 2;
#define UNDERSCORE 62
#define DASH 63

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
    uint h = 5381;
    for(int i = 0; word[i]; i++)
    {
        h = (h*HASH_A) + word[i]; //ALLOW OVERFLOW
    }
    return h;
}

char* uidof(char* a, char* b)
{
    char *first, *second;
    first = strcmp(a,b) < 0 ? a : b;
    second = first == a ? b : a;

    int l1 = strlen(first), l2 = strlen(second);
    char* uid = (char*)malloc(l1+l2+2);
    strcpy(uid, first);
    uid[l1] = '+';
    strcpy(uid + l1 + 1, second);
    uid[l1+l2+1] = '\0';

    return uid;
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
    hashtable ht = malloc(sizeof(struct _hashtable));
    ht->size=size;
    ht->buckets = (void**) calloc(size, sizeof(void*));
    ht->count = 0;
    return ht;
}

void ht_free(hashtable ht, int (*entry_free)(void*))
{
    for(int i = 0; ht->count > 0; i++) //IF STUFF CRASHES IT'S BECAUSE ht->count is wrong
    {
        if(ht->buckets[i] != NULL)
        {
            ht->count -= entry_free(ht->buckets[i]);
        }
    }
    free(ht->buckets);
    free(ht);
}