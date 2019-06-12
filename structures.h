#define MAXLEN 128

struct _link_item
{
    struct _link_item* next;
    char* uid;
    relarray relations;
};
typedef struct _link_item* link_item;

struct _ll_links
{
    link_item link;
    struct _ll_links* next;
};
typedef struct _ll_links* ll_node;

struct _ent_item
{
    struct _ent_item* next;
    char* id_ent;
    ll_node links;
    countarray relcounts;
};
typedef struct _ent_item* ent_item;

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
    struct _rel_item* next;
    char* id_rel;
    int index;
    int active_count;
    toplist top;
};
typedef struct _rel_item* rel_item;


ll_node ll_insert(ll_node root, link_item item)
{
    ll_node newnode = (ll_node) malloc(sizeof(struct _ll_links));
    newnode->next = root;
    newnode->link = item;
    return newnode;
}

link_item new_linkitem(char* uid)
{
    link_item item = (link_item) malloc(sizeof(struct _link_item));
    item->next = NULL;
    item->uid = uid;
    item->relations = new_relarray();
    return item;
}

link_item create_link(hashtable link_ht, ent_item ent_orig, ent_item ent_dest)
{
    //Calculate combined hash
    char* uid = uidof(ent_orig->id_ent, ent_dest->id_ent);

    uint h = hash(uid);
    //Search entry in link hashtable
    int index = h % link_ht->size;

    link_item link = NULL;
    link_item head_ptr = (link_item)link_ht->buckets[index];

    //If the bucket is not empty
    //Look for item in bucket with the uid we are looking for
    for(link_item current = head_ptr; current != NULL; current = current->next)
    {
        if(!strcmp(uid, current->uid))
        {
            link = current;
            free(uid);
            break;
        }
    }
    //If link is still NULL, we havent found it, so we create a new link item and place it in the list
    if(link == NULL)
    {
        link = new_linkitem(uid);
        link->next = head_ptr;
        head_ptr = link;

        //Add link pointer to both entities entries list
        ent_orig->links = ll_insert(ent_orig->links, link);
        //ll_insert(ent_dest->links, link);
    }
    //The bucket is empty, so we create a new link item and point the bucket to it
    else
    {
        link = new_linkitem(uid);
        link_ht->buckets[index] = link;
        
        //Add link pointer to both entities entries list
        //TODO Sometimes links not set
        ent_orig->links = ll_insert(ent_orig->links, link);
        //ent_dest->links = ll_insert(ent_dest->links, link);
    }
    return link;
}

int ht_link_free(void* entry)
{
    link_item item = (link_item) entry;
    int cnt = 1;
    if(item->next != NULL)
        cnt += ht_link_free(item->next);
    relarray_free(item->relations);
    free(item->uid);
    free(item);
    return cnt;
}

void ll_free(ll_node item)
{
    if(item->next != NULL) ll_free(item->next);
    ht_link_free(item->link); //TODO Check if this freeing is cool
    free(item);
}

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
    ent_item ent = malloc(sizeof(struct _ent_item));
    ent->next = NULL;
    ent->id_ent = strndup(id_ent, MAXLEN);
    ent->links = NULL;
    ent->relcounts = new_countarray();
    return ent;
}

int ht_ent_free(void* entry)
{
    ent_item item = (ent_item) entry;
    int cnt = 1;
    if(item->next != NULL) cnt += ht_ent_free(item->next);
    countarray_free(item->relcounts);
    free(item->id_ent);
    if (item->links != NULL) ll_free(item->links);
    free(item);
    return cnt;
}


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
