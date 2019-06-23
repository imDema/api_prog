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

struct _relation
{
    char* id_rel;
    int index;
    int active_count;
    toplist tl;
};
typedef struct _relation* relation;


struct _rel_db
{
    struct _relation* array;
    direct_ht ht;
    int size;
    int count;
};
typedef struct _rel_db* rel_db;

relation create_relation(rel_db relations, char* id_rel)
{
    //Check relation list in rel_ht
    uint h = hash(id_rel);

    relation rel = ht_search(relations->ht, id_rel, h);
    
    if(rel != NULL) return rel;

    //Create new
    if(relations->count == relations->size) //Expand if needed
    {
        relations->size *= 2;
        relations->array = realloc(relations->array, relations->size * sizeof(struct _relation));
    }
    
    //Set array element values
    rel = relations->array + relations->count;
    rel->id_rel = strndup(id_rel, MAXLEN);
    rel->active_count = 0;
    rel->index = relations->count;
    rel->tl.min_val = 0;
    rel->tl.head = NULL;
    rel->tl.count = 0;
    rel->tl.max_trigger = TOPLIST_MAX_TRIGGER;
    rel->tl.rebuild = 0;
    relations->count++; //Increase virtual size

    //Add array element to hashtable for lookups
    ht_insert(relations->ht, rel->id_rel, rel, h);

    return rel;
}

rel_db new_rel_db()
{
    rel_db relations = malloc(sizeof(struct _rel_db));
    relations->array = malloc(DEFAULT_REL_DB_ARR_SIZE * sizeof(struct _relation));
    relations->count = 0;
    relations->size  = DEFAULT_REL_DB_ARR_SIZE;
    relations->ht = new_direct_ht(DEFAULT_DIRECT_HT_SIZE);
    return relations;
}

void rel_db_free(rel_db relations)
{
    for(int i = 0; i < relations->count; i++)
    {
        free(relations->array[i].id_rel);
        //free toplist
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
