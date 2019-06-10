struct _link_item
{
    char* id_link;
    relarray relations;
    struct _link_item* next;
};
typedef struct _link_item* link_item;

link_item new_linkitem(char* uid)
{
    link_item item = (link_item) calloc(1,sizeof(struct _link_item));
    item->id_link = uid;
    return item;
}

void ht_link_free(void* entry)
{
    link_item item = (link_item) entry;
    if(item->next != NULL)
        ht_link_free(item->next);
    relarray_free(item->relations);
    free(item->id_link);
    free(item);
}

struct _ll_links
{
    link_item link;
    struct _ll_links* next;
};
typedef struct _ll_links* ll_node;

void ll_free(ll_node item)
{
    if(item->next != NULL) ll_free(item->next);
    ht_link_free(item->link);
}

ll_node ll_insert(ll_node root, link_item item)
{
    ll_node newnode = (ll_node) malloc(sizeof(struct _ll_links));
    newnode->next = root;
    newnode->link = item;
    return newnode;
}

struct _ent_item
{
    char* id_ent;
    ll_node links;
    countarray relcounts;
    struct _ent_item* next;
};
typedef struct _ent_item* ent_item;

void* ht_ent_search(hashtable ht ,char* word)
{
    uint h = hash(word);
    int bucket = h % ht->size;
    ent_item item = (ent_item) ht->buckets[bucket];
    while(item != NULL)
    {
        if(!strcmp(item->id_ent, word))
            return item;
        item = item->next;
    }
    return item;
}

ent_item new_ent_item(char* id_ent)
{
    ent_item ent = (ent_item)calloc(1,sizeof(struct _ent_item));
    ent->id_ent = id_ent;
    ent->relcounts = new_countarray();
    return ent;
}

void ht_ent_free(void* entry)
{
    ent_item item = (ent_item) entry;
    if(item->next != NULL) ht_ent_free(item->next);
    countarray_free(item->relcounts);
    free(item->id_ent);
    ll_free(item->links);
}

struct _toplist
{
    struct _toplist* next;
    int value;
    int size;
    int count;
    ent_item item;
};
typedef struct _toplist* toplist;

void tl_free(toplist tl)
{
    if(tl->next != NULL) tl_free(tl->next);
    free(tl);
}

struct _rel_item
{
    struct _rel_item* next;
    char* id_rel;
    int index;
    int active_count;
    toplist top;
};
typedef struct _rel_item* rel_item;

rel_item new_rel_item(hashtable ht, char* rel_id)
{
    rel_item item = calloc(1, sizeof(struct _rel_item));
    item->index = ht->count++;
    return item;
}

void ht_rel_free(void* entry)
{
    rel_item item = (rel_item) entry;
    tl_free(item->top);
    free(item->id_rel);
    free(item);
}
