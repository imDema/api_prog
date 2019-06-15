#define MAXLEN 128

/////////NEW

struct _entity
{
    direct_ht ht_links;
    struct _countarray in_counts;
};
typedef struct _entity* entity;

entity new_entity()
{
    return calloc(1, sizeof(struct _entity));
}

typedef struct _relation
{
    char* id_rel;
    int active_count;
    toplist tl;
} relation;

struct _rel_db
{
    relation* array;
    direct_ht ht;
    int orphaned;
};
typedef struct _rel_db* rel_db;

/////////NEW

struct _toplist
{
    struct _toplist* next;
    int value;
    int size;
    int count;
    entity item;
};
typedef struct _toplist* toplist;

struct _rel_item
{
    struct _rel_item* next;
    char* id_rel;
    int index;
    int active_count;
    toplist top;
};
typedef struct _rel_item* rel_item;

//TODO
void tl_free(toplist tl)
{
    //if(tl->next != NULL) tl_free(tl->next);
    free(tl);
}

rel_item new_rel_item(hashtable ht, char* rel_id)
{
    rel_item item = malloc(sizeof(struct _rel_item));
    item->next = NULL;
    item->id_rel = strndup(rel_id, MAXLEN);
    item->index = ht->count++;
    item->active_count = 0;
    item->top = NULL; //TODO INIT
    return item;
}

rel_item create_relation(hashtable rel_ht, char* id_rel)
{
    //Check relation list in rel_ht
    uint h = hash(id_rel);
    int index = h % rel_ht->size;

    rel_item item = rel_ht->buckets[index];
    //If the bucket is not empty
    if(item != NULL)
    {
        rel_item rel = (rel_item) rel_ht->buckets[index];
        if(!strcmp(id_rel, rel->id_rel))
            item = rel;
        else while (rel->next != NULL)
        {
            rel = rel->next;
            if(!strcmp(id_rel, rel->id_rel))
            {
                item = rel;
                break;
            }
        }
        
        //If it doesn't create a new entry and increment nonce
        if(item == NULL)
        {
            item = new_rel_item(rel_ht, id_rel);
            rel->next = item;
        }
    }
    //The bucket is empty, so we create a new relation item and point the bucket to it
    else
    {
        item = new_rel_item(rel_ht, id_rel);
        rel_ht->buckets[index] = item;
    }
    return item;
}

rel_item ht_rel_search(hashtable ht_rel, char* id_rel)
{
    uint index = hash(id_rel) % ht_rel->size;
    rel_item item = (rel_item)ht_rel->buckets[index];
    for(rel_item item = (rel_item)ht_rel->buckets[index]; item != NULL; item = item->next)
        if(!strcmp(id_rel, item->id_rel))
            return item;

    return NULL;
}

int ht_rel_free(void* entry)
{
    rel_item item = (rel_item) entry;
    int cnt = 1;
    if(item->next != NULL) cnt += ht_rel_free(item->next);
    tl_free(item->top);
    free(item->id_rel);
    free(item);
    return cnt;
}
