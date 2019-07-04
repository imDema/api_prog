#define HASH_A 33
#define HASH_B 101

const int primes_size = 72;
const int primes[] = {
            3, 7, 11, 17, 23, 29, 37, 47, 59, 71, 89, 107, 131, 163, 197, 239, 293, 353, 431, 521, 631, 761, 919,
            1103, 1327, 1597, 1931, 2333, 2801, 3371, 4049, 4861, 5839, 7013, 8419, 10103, 12143, 14591,
            17519, 21023, 25229, 30293, 36353, 43627, 52361, 62851, 75431, 90523, 108631, 130363, 156437,
            187751, 225307, 270371, 324449, 389357, 467237, 560689, 672827, 807403, 968897, 1162687, 1395263,
            1674319, 2009191, 2411033, 2893249, 3471899, 4166287, 4999559, 5999471, 7199369 };

#define LOADFAC 0.74f
#define DEFAULT_DIRECT_HT_SIZE 3

uint hash(char* word)
{
    uint h = 5381;
    for(int i = 0; word[i]; i++)
    {
        h = (h*HASH_A) + word[i]; //ALLOW OVERFLOW
    }
    return h;
}

typedef struct bucket //20 bytes each
{
    uint hash;
    char* key;
    void* value;
} bucket;

struct _direct_ht //20 bytes each
{
    int count;
    int size;
    int loadsize;
    int occupied;
    struct bucket* buckets;     //80 for default size
};
typedef struct _direct_ht* direct_ht;

bucket* new_buckets(int size)
{
    return calloc(size, sizeof(bucket));
}

int get_prime_size(int approx_size)
{
    int size = __INT_MAX__;
    for(int s = 0; s < primes_size; s++)
        if(primes[s] >= approx_size)
        {
            size = primes[s];
            break;
        }
    return size;
}

struct _direct_ht create_direct_ht(int min_size)
{

    int newsize = get_prime_size(min_size);
    struct _direct_ht new_ht;
    new_ht.count = 0;
    new_ht.size = newsize;
    new_ht.loadsize = newsize * LOADFAC;
    new_ht.occupied = 0;
    new_ht.buckets = new_buckets(newsize);
    return new_ht;
}

direct_ht new_direct_ht(int min_size)
{
    direct_ht ht = malloc(sizeof(struct _direct_ht));
    *ht = create_direct_ht(min_size);
    return ht;
}

void ht_put(direct_ht ht, char* key, void* value, uint hash)
{
    //Starting index
    int index = hash % ht->size;
    //Increments
    uint h2 = 1 + ((unsigned long) hash * HASH_B) % (ht->size - 1);
    //Find an open slot
    while(1)
    {
        if(ht->buckets[index].hash == 0)
            break;
        index = (index + h2) % ht->size;
    }
    ht->buckets[index].hash = hash;
    ht->buckets[index].key = key;
    ht->buckets[index].value = value;
    ht->count++;
    ht->occupied++; //TODO move up and put behind if key == NULL
}

void expand_hashtable(direct_ht ht)
{
    struct _direct_ht new_ht = create_direct_ht(ht->size * 2);

    for(int i = 0; i < ht->size; i++)
    {
        bucket bkt = ht->buckets[i];
        if(bkt.hash != 0)
        {
            ht_put(&new_ht, bkt.key, bkt.value, bkt.hash);
        }
    }
    ht->occupied = ht->count;

    free(ht->buckets);
    *ht = new_ht;
}

void* ht_search(direct_ht ht, char* key, uint hash)
{
    //Starting index
    int index = hash % ht->size;
    //Increments
    uint h2 = 1 + ((unsigned long) hash * HASH_B) % (ht->size - 1);
    
    while(1)
    {
        bucket bkt = ht->buckets[index];

        if(bkt.hash == hash && !strcmp(bkt.key, key)) //Found
            return bkt.value;

        else if(bkt.hash == 0 && bkt.value != ht->buckets) //Landed on empty no collisions bucket
            return NULL;

        index = (index + h2) % ht->size;
    }
}

char* ht_search_keyptr(direct_ht ht, char* key, uint hash)
{
    //Starting index
    int index = hash % ht->size;
    //Increments
    uint h2 = 1 + ((unsigned long) hash * HASH_B) % (ht->size - 1);
    
    while(1)
    {
        bucket bkt = ht->buckets[index];

        if(bkt.hash == hash && !strcmp(bkt.key, key)) //Found
            return bkt.key;

        else if(bkt.hash == 0 && bkt.value != ht->buckets) //Landed on empty no collisions bucket
            return NULL;

        index = (index + h2) % ht->size;
    }
}

void ht_insert(direct_ht ht, char* key, void* value, uint hash)
{
    if(ht_search(ht, key, hash) != NULL) return;

    if(ht->occupied >= ht->loadsize)
        expand_hashtable(ht);
    
    ht_put(ht, strndup(key, MAXLEN), value, hash);
}

void ht_delete(direct_ht ht, char* key, uint hash)
{
    //Starting index
    int index = hash % ht->size;
    //Increments
    uint h2 = 1 + ((unsigned long) hash * HASH_B) % (ht->size - 1);
    //Find an open slot
    while(1)
    {
        bucket bkt = ht->buckets[index];
        if(bkt.hash == hash && !strcmp(bkt.key, key)) //Found
        {
            //Free key, set hash to 0 and mark as deleted by setting value to ht->bucket
            ht->buckets[index].hash = 0;
            ht->buckets[index].value = ht->buckets; //Marked as deleted to keep potential colliding walks intact
            free(ht->buckets[index].key);
            ht->buckets[index].key = NULL;
            ht->count--;
            break;
        }
        else if(bkt.hash == 0 && bkt.value != ht->buckets) //Landed on empty no collisions bucket, item didn't exist
            break;

        index = (index + h2) % ht->size;
    }
}

void ht_free(direct_ht ht)
{
    if(ht->count > 0)
        for(int i = 0; i < ht->size; i++)
        {
            if(ht->buckets[i].key != NULL)
                free(ht->buckets[i].key);
        }
    free(ht->buckets);
    free(ht);
}