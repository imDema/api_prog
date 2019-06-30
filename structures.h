#define MAXLEN 128
#define DEFAULT_REL_DB_ARR_SIZE 4

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

typedef struct _toparray
{
    int value;
    char** array;
    int count;
    int size;
    byte valid;
    byte sorted;
} toparray;

struct _relation
{
    struct _relation* next;
    char* id_rel;
    int index;
    int active_count;
    toparray top;
};
typedef struct _relation relation;


struct _rel_db
{
    relation* list;
    relation** array;
    direct_ht ht;
    int size;
    int count;
    int top_valid;
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
    rel->active_count = 0;
    rel->index = relations->count;
    rel->top.count = 0;
    rel->top.value = 0;
    rel->top.size = 0;
    rel->top.valid = 1;
    rel->top.sorted = 0;
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
    relations->top_valid = 1;
    return relations;
}

void toparray_push(toparray* tarr, char* item)
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
    tarr->sorted = 0;
}

void toparray_update(relation* rel, int newval, char* key)
{
    if(rel->top.valid == 0) return;

    int tval = rel->top.value;
    if(newval < tval) return;

    if(newval > tval)
    {
        rel->top.value = newval;
        rel->top.count = 0;
    }

    toparray_push(&rel->top, key);
}

void toparray_remove(relation* rel, char* key)
{
    if(rel->top.valid == 0) return;

    char** ar = rel->top.array;
    for(int i = 0, m = rel->top.count; i < m; i++)
    {
        if(!strcmp(ar[i], key))
        {
            ar[i] = ar[m-1];
            rel->top.count--;
        }
    }
    if(rel->top.count == 0)
        rel->top.valid = 0;
    rel->top.sorted = 0;
}

void rel_db_free(rel_db relations)
{
    for(int i = 0; i < relations->count; i++)
    {
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
        free(ent->in_counts.array);

        free(bkt.key);
        free(bkt.value);
    }
    free(ht->buckets);
    free(ht);
}
