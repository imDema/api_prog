#define MAXLEN 128
#define DEFAULT_REL_DB_ARR_SIZE 2

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

struct _relation
{
    char* id_rel;
    int index;
    int active_count;
    //toplist tl;
};
typedef struct _relation* relation;


struct _rel_db
{
    struct _relation* array;
    direct_ht ht;
    int size;
    int count;
    int orphaned;
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
        relations->array = realloc(relations->array, 2 * relations->size * sizeof(struct _relation));
    
    //Set array element values
    rel = relations->array + relations->count;
    rel->id_rel = strndup(id_rel, MAXLEN);
    rel->index = relations->count;
    relations->count++; //Increase virtual size

    //Add array element to hashtable for lookups
    ht_insert(relations->ht, rel->id_rel, rel, h);

    return rel;
}

rel_db new_rel_db()
{
    rel_db relations = malloc(sizeof(struct _rel_db));
    relations->array = calloc(DEFAULT_REL_DB_ARR_SIZE, sizeof(struct _relation));
    relations->count = 0;
    relations->size  = DEFAULT_REL_DB_ARR_SIZE;
    relations->orphaned = 0;
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
}

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
