void addent(direct_ht* ht, const char* id_ent)
{
    //Calculate hash
    uint h = hash(id_ent);

    //Check if duplicate
    if(ht_search(ht,id_ent, h) != NULL)
        return;
    //Allocate a new entity and duplicate the string
    entity ent = new_entity(id_ent);

    ht_insert(ht, ent->id_ent, ent, h);
}

void addrel(direct_ht* ht, rel_db relations,
                const char* id_orig, const char* id_dest, const char* id_rel)
{
    //Check if both entities exist
    uint h_orig = hash(id_orig),
        h_dest = hash(id_dest);
    entity ent_orig = ht_search(ht, id_orig, h_orig),
        ent_dest = ht_search(ht, id_dest, h_dest);
    if(ent_orig == NULL || ent_dest == NULL)
        return;

    if(ent_orig->ht == NULL)
        ent_orig->ht = new_direct_ht(DEFAULT_DIRECT_HT_SIZE);
    if(ent_dest->ht == NULL)
        ent_dest->ht = new_direct_ht(DEFAULT_DIRECT_HT_SIZE);

    //Get relarray if it exists
    relarray rar = ht_search(ent_orig->ht, ent_dest->id_ent, h_dest);

    if(rar == NULL) //If the link didn't exist create a new one
    {
        //Create a new relarray and place it in both the entities trees
        rar = new_relarray();
        ht_insert(ent_orig->ht, ent_dest->id_ent, rar, h_dest);

        //Check if the relation is reflessive, if not don't add it twice
        if(ent_orig != ent_dest)
            ht_insert(ent_dest->ht, ent_orig->id_ent, rar, h_orig);
    }

    //Get relation item (create it if it doesn't exist)
    relation* rel = create_relation(relations, id_rel);

    int direction = strcmp(id_orig, id_dest);

    //Set active relation arraylist to proper value
    if(relarray_add(rar, rel->index, direction))
    {
        //If the relation didn't exist increase the count in the relation heap
        increase_relation_count(relations, rel->index, ent_dest->id_ent, h_dest);
    }
}

//For entity ent2 decrease all relation counters of active relations on link
void decrease_all_relations(rel_db relations, relarray rar, int order, const char* id_ent2, uint h_ent2)
{
    byte* ar = rar->array;
    byte mask = order <= 0 ? FROM_FIRST : FROM_SECOND;

    int m = rar->size < relations->maxindex ? rar->size : relations->maxindex;
    for(int index = 0; index < m; index++)
    {
        byte b = ar[index];
        if(b & mask)
        {
            //If an active relation on the link is entering the linked node decrease the counts
            decrease_relations_count(relations, index, id_ent2, h_ent2);
        }
    }
}

//Go through all the entity's active links, delete and update counters
void dellinks(const direct_ht* ht, rel_db relations, entity ent, const uint h_ent)
{
    //Scroll through all the elements
    direct_ht entity_ht = *ent->ht;
    for(int i = 0, cnt = entity_ht.count; cnt > 0; i++)
    {
        bucket bkt = entity_ht.buckets[i];
        if(bkt.key == NULL)
            continue; //Continue till a valid bucket
        
        cnt--;
        int order = strcmp(ent->id_ent, bkt.key);

        if(order == 0) //Same id => Reflessive relations
        {
            relarray_free((relarray)bkt.value);
            continue;
        }

        //Find linked entity
        const uint h_ent2 = hash(bkt.key);
        entity ent2 = ht_search(ht, bkt.key, h_ent2);

        //Empty relarray updating counters
        relarray rar = (relarray)bkt.value;

        decrease_all_relations(relations, rar, order, ent2->id_ent, h_ent2);
        ht_delete(ent2->ht, ent->id_ent, h_ent);
        //Free up
        if(ent2->ht->count == 0)
        {
            ht_free(ent2->ht);
            ent2->ht = NULL;
        }
        relarray_free(rar);
    }
}

void delent(direct_ht* ht, rel_db relations, const char* id_ent)
{
    //Go to the entry in the entity hashtable
    uint h_ent = hash(id_ent);
    entity ent = ht_search(ht, id_ent, h_ent);

    if(ent == NULL) return;

    //Update all the relation heaps for the entity deletion
    for(int index = 0, m = relations->maxindex; index < m; index++)
        delete_relation_count(relations, index, id_ent, h_ent);

    //Delete all links updating the heap in the meantime
    if(ent->ht != NULL)
        dellinks(ht, relations, ent, h_ent);

    //Delete the entity
    ht_delete(ht, ent->id_ent, h_ent);
    //Free the resources
    ht_free(ent->ht);
    free(ent->id_ent);
    free(ent);
}

void delrel(direct_ht* ht, rel_db relations, const char* id_orig, const char* id_dest, const char* id_rel)
{
    //Calculate hashes and verify existence
    uint h_orig = hash(id_orig),
        h_dest = hash(id_dest);
    entity ent_orig = ht_search(ht, id_orig, h_orig),
        ent_dest = ht_search(ht, id_dest, h_dest);
    if(ent_orig == NULL || ent_dest == NULL || ent_orig->ht == NULL)
        return;
    
    //Find the relarray
    relarray rar = ht_search(ent_orig->ht, ent_dest->id_ent, h_dest);
    if(!rar) return;

    //Find the relation
    relation* rel = aa_search(relations->tree, id_rel);
    if(!rel) return;

    //Try removing
    int direction = strcmp(id_orig, id_dest);
    if(relarray_remove(rar, rel->index, direction))
    {
        //If there actually was a relation decrease the entry in the heap
        decrease_relations_count(relations, rel->index, id_dest, h_dest);
    }
    if(rar->count == 0) //If there are no more active relations on the link delete it
    {
        //Delete the link from the origin
        ht_delete(ent_orig->ht, ent_dest->id_ent, h_dest);
        if(ent_orig->ht->count == 0)
        {
            ht_free(ent_orig->ht);
            ent_orig->ht = NULL;
        }
        //If the relation is not reflessive delete the link from the destination
        if(ent_orig != ent_dest)
        {
            ht_delete(ent_dest->ht, ent_orig->id_ent, h_orig);
            if(ent_dest->ht->count == 0)
            {
                ht_free(ent_dest->ht);
                ent_dest->ht = NULL;
            }
        }
        relarray_free(rar);
    }
}

# define INT_MAX_STRING_LENGTH 11 //MAX_INT is 10 digits

//Fast int formatter because printf is too generic and slow
void fast_int_format(char* string, int integer)
{
    char formatted[INT_MAX_STRING_LENGTH]; 
    int s = 0;
    for(int val = integer; val > 0; val /= 10)
        formatted[s++] = val % 10 + '0';
    
    for(int i = s - 1; i >= 0; i--)
        *string++ = formatted[i];
    
    *string = '\0';
}

int cmpstr(const void* a, const void* b)
{
    const char* aa = *(const char**)a;
    const char* bb = *(const char**)b;
    return strcmp(aa,bb);
}

void rebuild_top(relation* rel)
{
    rel->topval = hh_peek(&(rel->hheap));   //Peek the top value from the heap
    arraylist_reset(&(rel->top));   //Rebuild the list
    hh_addto_topar(&(rel->hheap), &(rel->top), rel->topval, 0);
    qsort(rel->top.array, rel->top.count, sizeof(char*), cmpstr);   //Sort it
}

void sorted_print(aa_node tree, int* first)
{
    //Depth first traversal for sorted access
    if(tree == NULL) return;
    sorted_print(tree->left, first);

    char intbuf[INT_MAX_STRING_LENGTH];

    relation* rel = tree->content;
    if(rel->hheap.binheap.count > 0) //If there are active relations
    {
        if(rel->topval <=  0)   //A relations' topval gets set to -1 when the last toplist is invalid
            rebuild_top(rel);   //and needs to be rebuilt

        intbuf[0] = '\0';
        if(!*first)
            fputc(' ', stdout);
        *first = 0;
        fputc('\"', stdout);    //Relation id
        fputs(rel->id_rel, stdout);
        fputs("\" ", stdout);
        for(int j = 0; j < rel->top.count; j++) //Ent_id in toplist
        {
            fputc('\"', stdout);
            fputs(rel->top.array[j], stdout);
            fputs("\" ", stdout);
        }
        fast_int_format(intbuf, rel->topval); //Top count
        fputs(intbuf, stdout);
        fputc(';', stdout);
    }

    sorted_print(tree->right, first);
}

void report(rel_db relations)
{
    //Used for dividing spaces
    int first = 1;

    sorted_print(relations->tree, &first);
    if(first)
        fputs("none\n", stdout);
    else
        fputc('\n', stdout);
}
