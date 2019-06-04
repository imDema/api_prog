#include <stdlib.h>
#include "arraylist.h"

struct _link_item
{
    struct _link_item* next;
    relarray relations;
};
typedef struct _link_item* link_item;

void ht_link_free(void* entry)
{
    link_item item = (link_item) entry;
    if(item->next != NULL)
        ht_link_free(item->next);
    relarray_free(item->relations);
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
    ll_node_links links;
    countarray relcounts;
};
typedef struct _ent_item* ent_item;

void ht_ent_free(void* entry)
{
    ent_item item = (ent_item) entry;
    countarray_free(item->relcounts);
    ll_free(item->links);
}

void ht_rel_free(void* entry);