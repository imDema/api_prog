#define MAXLEN 128
#define DEFAULT_REL_DB_ARR_SIZE 4
#define DEFAULT_HARR_SIZE 4

/////////NEW

struct _entity
{
    direct_ht ht_links;
};
typedef struct _entity* entity;

entity new_entity()
{
    return calloc(1, sizeof(struct _entity));
}

typedef struct heap_item
{
    char* id_ent;
    int count;
    int hashpos;
} heap_item;

typedef struct heap
{
    int size;
    int count;
    heap_item** harr;
} heap;

typedef struct hashheap
{
    direct_ht* ht;
    heap binheap;
} hashheap;

int parent(int i)
{
    return (i-1)/2;
}
int left(int i)
{
    return 2*i+1;
}
int right (int i)
{
    return 2+i+2;
}

void heap_swap(heap* binheap, int a, int b)
{
    binheap->harr[a]->hashpos = b;
    binheap->harr[b]->hashpos = a;
    heap_item* tmp = binheap->harr[a];
    binheap->harr[a] = binheap->harr[b];
    binheap->harr[b] = tmp;
}

void hh_increase(hashheap* hh, char* id_ent, uint hash)
{
    heap_item* item = ht_search(hh->ht, id_ent, hash);

    if(item == NULL)
    {
        if(hh->binheap.size == 0)
        {
            hh->binheap.size = DEFAULT_HARR_SIZE;
            hh->binheap.harr = malloc(DEFAULT_HARR_SIZE * sizeof(heap_item*));
        }
        else if(hh->binheap.count == hh->binheap.size)
        {
            hh->binheap.size <<= 1;
            hh->binheap.harr = realloc(hh->binheap.harr, hh->binheap.size * sizeof(heap_item*));
        }

        heap_item* newitem = malloc(sizeof(heap_item));
        ht_insert(hh->ht, id_ent, newitem, hash);
        newitem->count = 1;
        newitem->id_ent = id_ent;
        
        int pos = hh->binheap.count++;
        newitem->hashpos = pos;
        hh->binheap.harr[pos] = newitem;
    }
    else
    {
        item->count++;
        int pos = item->hashpos;
        heap_item** harr = hh->binheap.harr;
        
        while (pos != 0 && harr[parent(pos)] < harr[pos])
        {
            heap_swap(&(hh->binheap), pos, parent(pos));
            pos = parent(pos);
        }
    }
}

void max_heapify(heap* binheap, int pos)
{
    int l = left(pos);
    int r = right(pos);
    int greatest = pos;

    if (l < binheap->count && binheap->harr[l] > binheap->harr[greatest]) 
        greatest = l; 
    if (r < binheap->count && binheap->harr[r] > binheap->harr[greatest]) 
        greatest = r; 

    if (greatest != pos)
    {
        heap_swap(binheap, greatest, pos);
        max_heapify(binheap, greatest);
    }
}

void hh_delete(hashheap* hh, char* id_ent, uint hash)
{
    heap_item* item = ht_search(hh->ht, id_ent, hash);
    if(item == NULL) fputs("ERROR, trying to decrease not existing item in heap", stderr);
    for(int pos = item->hashpos; pos != 0; pos = parent(pos))
        heap_swap(&(hh->binheap), pos, parent(pos));
    
    heap_swap(&(hh->binheap), 0, hh->binheap.count - 1);
    hh->binheap.count--;
    max_heapify(&(hh->binheap), 0);
}

void hh_decrease(hashheap* hh, char* id_ent, uint hash)
{
    heap_item* item = ht_search(hh->ht, id_ent, hash);
    if(item == NULL) fputs("ERROR, trying to decrease not existing item in heap", stderr);
    if(item->count == 1)
        hh_delete(hh, id_ent, hash);
    else
    {
        item->count--;
        max_heapify(&(hh->binheap), item->hashpos);
    }
}

int hh_peek(hashheap* hh)
{
    return hh->binheap.harr[0]->count;
}

void hh_addto_topar(hashheap* hh, arraylist* topar, int count, int root)
{
    if(hh->binheap.harr[root] == count)
    {
        arraylist_insert(topar, hh->binheap.harr[root]->id_ent);
        hh_addto_topar(hh,topar,count,left(root));
        hh_addto_topar(hh,topar,count,right(root));
    }
}

//TODO:
/*
Implement pointer based binary heap
Point to nodes using a hashtable (key = id_ent)
Increment:
    Search in ht for id
        Exists? Go to node, increment, balance heap
        Doesn't? Create node, add to ht, Insert in heap

Decrement:
    Search in ht for id
        Doesn't exist? return
        Exists?
            Decrement
                Count == 0? Delete from heap, delete from ht
                Count > 0? Balance heap

Peek:
    Return root
 */

struct _relation
{
    struct _relation* next;
    char* id_rel;
    int index;
    int valid_top;
    arraylist top;
    hashheap hheap;
};
typedef struct _relation relation;

struct _rel_db
{
    relation* list;
    relation** array;
    direct_ht ht;
    int size;
    int count;
};
typedef struct _rel_db* rel_db;

relation* create_relation(rel_db relations, char* id_rel)
{
    //Check relation list in rel_ht
    uint h = hash(id_rel);

    relation* rel = ht_search(relations->ht, id_rel, h);
    
    if(rel != NULL) return rel;

    //Create new node
    rel = malloc(sizeof(relation));
    rel->id_rel = strndup(id_rel, MAXLEN);
    rel->index = relations->count;
    //TODO INIT TOPLIST

    //Insert in linked list sorting by id_rel
    if(relations->list == NULL || strcmp(id_rel, relations->list->id_rel) < 0)
    {
        rel->next = relations->list;
        relations->list = rel;
    }
    else
    {
        relation* curr = relations->list;
        while(curr->next != NULL && strcmp(id_rel, curr->next->id_rel) > 0)
            curr = curr->next;

        rel->next = curr->next;
        curr->next = rel;
    }
    

    //Add pointer to direct access array
    if(relations->count == relations->size) //Expand if needed
    {
        relations->size *= 2;
        relations->array = realloc(relations->array, relations->size * sizeof(relation*));
    }
    relations->array[relations->count] = rel;
    relations->count++; //Increase virtual size

    //Add pointer to hashtable for string lookups
    ht_insert(relations->ht, rel->id_rel, rel, h);

    return rel;
}

rel_db new_rel_db()
{
    rel_db relations = malloc(sizeof(struct _rel_db));
    relations->list = NULL;
    relations->array = malloc(DEFAULT_REL_DB_ARR_SIZE * sizeof(relation*));
    relations->ht = new_direct_ht(DEFAULT_DIRECT_HT_SIZE);
    relations->count = 0;
    relations->size  = DEFAULT_REL_DB_ARR_SIZE;
    return relations;
}

void rel_db_free(rel_db relations)
{
    for(int i = 0; i < relations->count; i++)
    {
        free(relations->array[i]->top.array);
        free(relations->array[i]->id_rel);
        free(relations->array[i]);
    }
    free(relations->array);
    ht_free(relations->ht);
    free(relations);
}

void free_prev_relrrays(direct_ht ht, bucket bktdelete)
{
    entity ent = bktdelete.value;

    //Delete the links
    if(ent->ht_links != NULL)
    for(int i = 0; i < ent->ht_links->size; i++)
    {
        bucket bkt = ent->ht_links->buckets[i];
        if(bkt.hash != 0) //If a valid link has been found
        {
            if(strcmp(bkt.key, bktdelete.key) <= 0)
            {
                relarray_free(bkt.value);
            }
            free(bkt.key);
        }
    }
}

void free_entities(direct_ht ht)
{
    for(int i = 0; i < ht->size; i++)
    {
        bucket bkt = ht->buckets[i];
        if(bkt.hash == 0)
            continue;

        free_prev_relrrays(ht, bkt);
        entity ent = bkt.value;
        if(ent->ht_links != NULL)
        {
            free(ent->ht_links->buckets);
            free(ent->ht_links);
        }

        free(bkt.key);
        free(bkt.value);
    }
    free(ht->buckets);
    free(ht);
}
