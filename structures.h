#define DEFAULT_REL_DB_ARR_SIZE 4
#define DEFAULT_HARR_SIZE 4
#define TOPVAL_INVALID -1

/////////NEW

typedef struct heap_item
{
    const char* id_ent;
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

struct _aa_node
{
    const char* key;
    void* content;

    int level;
    struct _aa_node* left;
    struct _aa_node* right;
};
typedef struct _aa_node* aa_node;

struct _entity
{
    char* id_ent;
    direct_ht* ht;
};
typedef struct _entity* entity;

typedef struct pq
{
    int* array;
    int size;
    int count;
} pq;

void swapint(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

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
    return 2*i+2;
}

void min_heapifypq(pq queue, int pos)
{
    int l = left(pos);
    int r = right(pos);
    int smallest = pos;

    if (l < queue.count && queue.array[l] < smallest) 
        smallest = l; 
    if (r < queue.count && queue.array[r] < smallest) 
        smallest = r; 

    if (smallest != pos)
    {
        swapint(&queue.array[smallest], &queue.array[pos]);
        min_heapifypq(queue, smallest);
    }
}

void pq_push(pq* queue, int value)
{
    if(queue->size == queue->count)
    {
        queue->size *= 2;
        queue->array = realloc(queue->array, queue->size * sizeof(int));
    }
    queue->array[queue->count++] = value;
    for(int p = queue->count-1; p != 0 && queue->array[p] < queue->array[parent(p)]; p = parent(p))
        swapint(&queue->array[p], &queue->array[parent(p)]);
}

int pq_pop(pq* queue)
{
    if(queue->count == 0)
        return -1;
    int v = queue->array[0];
    swapint(&queue->array[0], &queue->array[--queue->count]);
    min_heapifypq(*queue, 0);
    return v;
}

entity new_entity(const char* id_ent)
{
    entity ent = malloc(sizeof(struct _entity));
    ent->id_ent = strndup(id_ent, MAXLEN);
    ent->ht = NULL;
    return ent;
}

aa_node skew(aa_node node)
{
    if(node == NULL || node->left == NULL)
        return node;

    if(node->left->level == node->level)
    {
        aa_node left = node->left;
        node->left = left->right;
        left->right = node;
        return left;
    }
    else return node;
}

aa_node split(aa_node node)
{
    if(node == NULL || node->right == NULL || node->right->right == NULL)
        return node;
    if(node->level == node->right->right->level)
    {
        aa_node right = node->right;
        node->right = right->left;
        right->left = node;
        right->level++;
        return right;
    }
    else return node;
}

aa_node aa_insert(aa_node tree, const char* key, void* value)
{
    if(tree == NULL)
    {
        aa_node newnode = malloc(sizeof(struct _aa_node));
        newnode->key = key;
        newnode->content = value;
        newnode->left = NULL;
        newnode->right = NULL;
        newnode->level = 1;
        return newnode;
    }
    else if(strcmp(key, tree->key) < 0)
        tree->left = aa_insert(tree->left, key, value);
    else if(strcmp(key, tree->key) > 0)
        tree->right = aa_insert(tree->right, key, value);
    else
        return tree;
    tree = skew(tree);
    tree = split(tree);
    return tree;
}

aa_node aa_pred(aa_node node)
{
    aa_node curr = node->left;
    while(curr->right != NULL)
        curr = curr->right;
    return curr;
}
aa_node aa_succ(aa_node node)
{
    aa_node curr = node->right;
    while(curr->left != NULL)
        curr = curr->left;
    return curr;
}

void aa_decrease_level(aa_node tree)
{
    int left = tree->left != NULL ? tree->left->level : 0;
    int right = tree->right != NULL ? tree->right->level : 0;

    int correct = left < right ? left + 1 : right + 1;
    if(tree->level > correct)
    {
        tree->level = correct;

        if(right > 0 && tree->right->level > correct)
            tree->right->level = correct;
    }
}

aa_node aa_delete(aa_node tree, const char* key)
{
    //Search
    if(tree == NULL)
        return NULL;

    else if(strcmp(key, tree->key) < 0)
        tree->left = aa_delete(tree->left, key);
    else if(strcmp(key, tree->key) > 0)
        tree->right = aa_delete(tree->right, key);
    else
    {
        //Delete
        if(tree->left == NULL && tree->right == NULL)
        {
            free(tree);
            return NULL;
        }
        else if(tree->left == NULL)
        {
            aa_node succ = aa_succ(tree);
            struct _aa_node s_val = *succ;
            tree->right = aa_delete(tree->right, succ->key);
            tree->key = s_val.key;
            tree->content = s_val.content;
        }
        else
        {
            aa_node pred = aa_pred(tree);
            struct _aa_node p_val = *pred;
            tree->left = aa_delete(tree->left, p_val.key);
            tree->key = p_val.key;
            tree->content = p_val.content;
        }
    }
    //Rebalance
    aa_decrease_level(tree);
    tree = skew(tree);
    tree->right = skew(tree->right);
    if(tree->right != NULL)
        tree->right->right = skew(tree->right->right);
    tree = split(tree);
    tree->right = split(tree->right);
    return tree;
}

void* aa_search(aa_node tree, const char* key)
{
    if(tree == NULL)
        return NULL;
    else if(strcmp(key, tree->key) < 0)
        return aa_search(tree->left, key);
    else if(strcmp(key, tree->key) > 0)
        return aa_search(tree->right, key);
    else
        return tree->content;
}

void heap_swap(heap* binheap, int a, int b)
{
    if(a == b) return;
    binheap->harr[a]->hashpos = b;
    binheap->harr[b]->hashpos = a;
    heap_item* tmp = binheap->harr[a];
    binheap->harr[a] = binheap->harr[b];
    binheap->harr[b] = tmp;
}

//Returns new value
int hh_increase(hashheap* hh, const char* id_ent, uint hash)
{
    if(hh->ht == NULL)
        hh->ht = new_direct_ht(DEFAULT_DIRECT_HT_SIZE);
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
            hh->binheap.size *= 2;
            hh->binheap.harr = realloc(hh->binheap.harr, hh->binheap.size * sizeof(heap_item*));
        }

        item = malloc(sizeof(heap_item));
        ht_insert(hh->ht, id_ent, item, hash);
        item->count = 1;
        item->id_ent = ht_search_keyptr(hh->ht, id_ent, hash);
        
        int pos = hh->binheap.count++;
        item->hashpos = pos;
        hh->binheap.harr[pos] = item;
    }
    else
    {
        item->count++;
        int pos = item->hashpos;
        heap_item** harr = hh->binheap.harr;
        
        while (pos != 0 && harr[parent(pos)]->count < harr[pos]->count)
        {
            heap_swap(&(hh->binheap), pos, parent(pos));
            pos = parent(pos);
        }
    }
    return item->count;
}

void max_heapify(heap* binheap, int pos)
{
    int l = left(pos);
    int r = right(pos);
    int greatest = pos;

    heap_item** harr = binheap->harr;

    if (l < binheap->count && harr[l]->count > harr[greatest]->count) 
        greatest = l; 
    if (r < binheap->count && harr[r]->count > harr[greatest]->count) 
        greatest = r; 

    if (greatest != pos)
    {
        heap_swap(binheap, greatest, pos);
        max_heapify(binheap, greatest);
    }
}

//Returns old value
int hh_delete(hashheap* hh, const char* id_ent, uint hash)
{
    if(hh->ht == NULL) return 0;
    heap_item* item = ht_search(hh->ht, id_ent, hash);
    if(item == NULL) return 0;
    int oldval = item->count;
    for(int pos = item->hashpos; pos != 0; pos = parent(pos))
        heap_swap(&(hh->binheap), pos, parent(pos));
    
    heap_swap(&(hh->binheap), 0, hh->binheap.count - 1);
    hh->binheap.count--;
    max_heapify(&(hh->binheap), 0);
    ht_delete(hh->ht, id_ent, hash);
    free(item);
    if(hh->binheap.count < hh->binheap.size / 4)
    {
        hh->binheap.size /= 4;
        hh->binheap.harr = realloc(hh->binheap.harr, hh->binheap.size * sizeof(heap_item*));
    }

    return oldval;
}

//Returns new value
int hh_decrease(hashheap* hh, const char* id_ent, uint hash)
{
    heap_item* item = ht_search(hh->ht, id_ent, hash);
    if(item == NULL)
        return -1;
    if(item->count == 1)
        hh_delete(hh, id_ent, hash);
    else
    {
        item->count--;
        max_heapify(&(hh->binheap), item->hashpos);
        return item->count;
    }
    return 0;
}

int hh_peek(hashheap* hh)
{
    return hh->binheap.harr[0]->count;
}

void hh_addto_topar(hashheap* hh, arraylist* topar, int count, int root)
{
    if(root < hh->binheap.count && hh->binheap.harr[root]->count == count)
    {
        arraylist_insert(topar, hh->binheap.harr[root]->id_ent);
        hh_addto_topar(hh,topar,count,left(root));
        hh_addto_topar(hh,topar,count,right(root));
    }
}

struct _relation
{
    char* id_rel;
    int index;
    int topval;
    arraylist top;
    hashheap hheap;
};
typedef struct _relation relation;

struct _rel_db
{
    relation** array;
    aa_node tree;
    pq index_queue;
    int size;
    int maxindex;
};
typedef struct _rel_db* rel_db;

relation* create_relation(rel_db relations, const char* id_rel)
{
    relation* rel = aa_search(relations->tree, id_rel);
    
    if(rel != NULL) return rel;

    //Create new node
    rel = calloc(1, sizeof(relation));
    rel->id_rel = strndup(id_rel, MAXLEN);
    int index = pq_pop(&relations->index_queue);
    if(index >= 0)
    {
        rel->index = index;
        relations->array[index] = rel;
    }
    else
    {
        if(relations->maxindex == relations->size) //Expand if needed
        {
            relations->size *= 2;
            relations->array = realloc(relations->array, relations->size * sizeof(relation*));
        }
        rel->index = relations->maxindex++;
        relations->array[rel->index] = rel;
    }
    
    //Insert in linked list sorting by id_rel
    relations->tree = aa_insert(relations->tree, rel->id_rel, rel);

    return rel;
}

void delete_relation(rel_db relations, int index)
{
    relation* rel = relations->array[index];
    relations->array[index] = NULL;
    relations->tree = aa_delete(relations->tree, rel->id_rel);

    pq_push(&relations->index_queue, index);
    ht_free(rel->hheap.ht);
    free(rel->hheap.binheap.harr);
    free(rel->top.array);
    free(rel->id_rel);
    free(rel);
}

void decrease_relations_count(rel_db relations, int index, const char* id_ent, uint h_ent)
{
    int newval = hh_decrease(&(relations->array[index]->hheap), id_ent, h_ent);
    if (newval + 1 == relations->array[index]->topval)
        relations->array[index]->topval = TOPVAL_INVALID;

    if(newval == 0 && relations->array[index]->hheap.binheap.count == 0)
        delete_relation(relations, index);
}

void increase_relation_count(const rel_db relations, int index, const char* id_ent, uint h_ent)
{
    int newval = hh_increase(&(relations->array[index]->hheap), id_ent, h_ent);
    if(newval >= relations->array[index]->topval) // If item was in top list or should enter it
        relations->array[index]->topval = TOPVAL_INVALID;
}

void delete_relation_count(const rel_db relations, int index, const char* id_ent, uint h_ent)
{
    if(relations->array[index] == NULL) return;
    int oldval = hh_delete(&(relations->array[index]->hheap), id_ent, h_ent);
    if(oldval == relations->array[index]->topval) // If item was in top list
        relations->array[index]->topval = TOPVAL_INVALID;

    if(relations->array[index]->hheap.binheap.count == 0)
        delete_relation(relations, index);
}

rel_db new_rel_db()
{
    rel_db relations = malloc(sizeof(struct _rel_db));
    relations->tree = NULL;
    relations->array = malloc(DEFAULT_REL_DB_ARR_SIZE * sizeof(relation*));
    relations->index_queue.size = 2;
    relations->index_queue.array = malloc(sizeof(int) * relations->index_queue.size);
    relations->index_queue.count = 0;
    relations->maxindex = 0;
    relations->size  = DEFAULT_REL_DB_ARR_SIZE;
    return relations;
}

void aa_free(aa_node tree)
{
    if(tree == NULL)
        return;
    aa_free(tree->left);
    aa_free(tree->right);
    free(tree);
}


void free_relations(aa_node tree)
{
    if(tree == NULL) return;
    free_relations(tree->left);
    free_relations(tree->right);
    relation* rel = tree->content;

    ht_free(rel->hheap.ht);
    for(int j = 0, m = rel->hheap.binheap.count; j < m; j++)
    {
        free(rel->hheap.binheap.harr[j]);
    }
    free(rel->hheap.binheap.harr);
    free(rel->top.array);
    free(rel->id_rel);
    free(rel);
    free(tree);
}
void rel_db_free(rel_db relations)
{
    free_relations(relations->tree);
    
    free(relations->array);
    aa_free(relations->tree);
    free(relations);
}

void free_prev_relrrays(direct_ht* ht, const char* free_delimiter)
{
    bucket* buckets = ht->buckets;
    for(int i = 0, cnt = ht->count; cnt > 0; i++)
    {
        bucket bkt = buckets[i];
        if(bkt.key == NULL)
            continue;

        cnt--;
        if(strcmp(bkt.key, free_delimiter) <= 0)
            relarray_free(bkt.value);
    }
}

void free_entities(direct_ht* ht)
{
    bucket* buckets = ht->buckets;
    int pass2_index = 0;
    for(int i = 0, cnt = ht->count; cnt > 0; i++)
    {
        bucket bkt = buckets[i];
        if(bkt.key == NULL)
            continue;
        
        cnt--;
        entity ent = bkt.value;
        if(ent->ht != NULL)
        {
            free_prev_relrrays(ent->ht, ent->id_ent);
        }
        ht_free(ent->ht);
        //Group buckets at the beginning of the array for a faster 2nd pass
        buckets[pass2_index++] = buckets[i];
    }

    for(int i = 0; i < pass2_index; i++)
    {
        entity ent = buckets[i].value;
        free(ent->id_ent);
        free(ent);
    }
    free(ht->buckets);
    free(ht);
}
