#include <stdlib.h>
#include "arraylist.h"

struct _link_item
{
    char* id_link;
    relarray relations;
    struct _link_item* next;
};
typedef struct _link_item* link_item;

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
typedef struct _ll_links* ll_node_links;

void ll_free(ll_node_links item)
{
    if(item->next != NULL) ll_free(item->next);
    ht_link_free(item->link);
}

struct _ent_item
{
    char* id_ent;
    ll_node_links links;
    countarray relcounts;
    struct _ent_item* next;
};
typedef struct _ent_item* ent_item;

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

struct _rel_item
{
    char* id_rel;
    int index;
    int active_count;
    toplist top;
};
typedef struct _rel_item* rel_item;

void ht_rel_free(void* entry)
{
    rel_item item = (rel_item) entry;
    //TODO free toplsìist
    free(item->id_rel);
    free(item);
}