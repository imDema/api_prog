void addent(direct_ht ht, char* id_ent)
{
    //Calculate hash
    uint h = hash(id_ent);

    //Check if duplicate
    if(ht_search(ht,id_ent, h) != NULL)
        return;

    //Create new and insert in ht
    entity ent = new_entity();
    ht_insert(ht, id_ent, ent, h);
}

void addrel(direct_ht ht, rel_db relations,
                char* id_orig, char* id_dest, char* id_rel)
{
    //Check if both entities exist
    uint h_orig = hash(id_orig),
        h_dest = hash(id_dest);
    entity ent_orig = ht_search(ht, id_orig, h_orig),
        ent_dest = ht_search(ht, id_dest, h_dest);
    if(ent_orig == NULL || ent_dest == NULL)
        return;
    
    //Initialize link ht if needed
    if(ent_orig->ht_links == NULL)
        ent_orig->ht_links = new_direct_ht(DEFAULT_DIRECT_HT_SIZE);
    if(ent_dest->ht_links == NULL)
        ent_dest->ht_links = new_direct_ht(DEFAULT_DIRECT_HT_SIZE);

    //Get the relation array for the link
    relarray rar = ht_search(ent_orig->ht_links, id_dest, h_dest);
    if(rar == NULL)
    {
        //Initialize it if needed
        rar = new_relarray();
        ht_insert(ent_dest->ht_links, id_orig, rar, h_orig);
        ht_insert(ent_orig->ht_links, id_dest, rar, h_dest);
    }

    //Get relation info
    relation* rel = create_relation(relations, id_rel);

    int direction = strcmp(id_orig, id_dest);

    //Set active relation arraylist to proper value
    if(relarray_add(rar, rel->index, direction))
    {
        int newval = hh_increase(&(rel->hheap), id_dest, h_dest);
        if(newval >= rel->topval) // If item was in top list or should enter it
            rel->topval = TOPVAL_INVALID;
    }
}

void dellink(direct_ht ht, rel_db relations, char* id_ent, uint h_ent, bucket bkt)
{
    entity dest = ht_search(ht, bkt.key, bkt.hash);

    //Remove all outbound relations
    relarray rar = bkt.value;
    if(rar->count > 0) //Decrease count for every active relation on link
    {
        int order = strcmp(id_ent, bkt.key);
        byte mask = order <= 0 ? FROM_FIRST : FROM_SECOND;

        int lim = rar->size < relations->count ? rar->size : relations->count;
        for(int index = 0; index < lim; index++)
        {
            //If there is a relation entering the entity we are deleting
            byte b = rar->array[index];
            if(b & mask)
            {
                int newval = hh_decrease(&(relations->array[index]->hheap), bkt.key, bkt.hash);
                if(newval + 1 == relations->array[index]->topval) // If item was in top list
                    relations->array[index]->topval = TOPVAL_INVALID;
            }
        }
    }

    relarray_free(rar);
    ht_delete(dest->ht_links, id_ent, h_ent);
}

void delent(direct_ht ht, rel_db relations, char* id_ent)
{
    //Go to the entry in the entity hashtable
    uint h_ent = hash(id_ent);
    entity ent = ht_search(ht, id_ent, h_ent);

    if(ent == NULL) return;

    //Update all outbound links relation counts and delete the links
    direct_ht ent_ht = ent->ht_links;
    if(ent_ht != NULL)
    {
        if(ent_ht->count > 0)
        for(int i = 0, m = ent_ht->size; i < m; i++)
        {
            bucket bkt = ent_ht->buckets[i];
            if(bkt.hash != 0) //If a valid link has been found
            {
                dellink(ht, relations, id_ent, h_ent, bkt);
            }
        }
        ht_free(ent->ht_links);
    }
    
    for(int i = 0, m = relations->count; i < m; i++)
    {
        int oldval = hh_delete(&(relations->array[i]->hheap), id_ent, h_ent);
        if(oldval == relations->array[i]->topval) //If item was in top list
            relations->array[i]->topval = TOPVAL_INVALID;
    }

    free(ent);

    ht_delete(ht, id_ent, h_ent);
}

void delrel(direct_ht ht, rel_db relations, char* id_orig, char* id_dest, char* id_rel)
{
    //Calculate hashes and verify existence
    uint h_orig = hash(id_orig),
        h_dest = hash(id_dest);
    entity ent_orig = ht_search(ht, id_orig, h_orig),
        ent_dest = ht_search(ht, id_dest, h_dest);
    if(ent_orig == NULL || ent_dest == NULL)
        return;
    

    //Search link
    if(ent_orig->ht_links == NULL) return;
    relarray rar = ht_search(ent_orig->ht_links, id_dest, h_dest);
    
    //Skip if it does not exist
    if(!rar) return;

    //Update arraylist entry if needed
    relation* rel = ht_search(relations->ht, id_rel, hash(id_rel));
    if(!rel) return;

    int direction = strcmp(id_orig, id_dest);
    if(relarray_remove(rar, rel->index, direction)) //If the relation existed remove it and update counts
    {
        int newval = hh_decrease(&(rel->hheap), id_dest, h_dest);
        if (newval + 1 == rel->topval)
            rel->topval = TOPVAL_INVALID;
    }
    if(rar->count == 0) //Free the link
    {
        ht_delete(ent_orig->ht_links, id_dest, h_dest);
        ht_delete(ent_dest->ht_links, id_orig, h_orig);
        relarray_free(rar);
    }
}

int cmpstr(const void* a, const void* b)
{
    const char* aa = *(const char**)a;
    const char* bb = *(const char**)b;
    return strcmp(aa,bb);
}

void rebuild_top(relation* rel)
{
    rel->topval = hh_peek(&(rel->hheap));
    arraylist_reset(&(rel->top));
    hh_addto_topar(&(rel->hheap), &(rel->top), rel->topval, 0);
    qsort(rel->top.array, rel->top.count, sizeof(char*), cmpstr);
}

void report(rel_db relations)
{
    char output[512];
    output[0] = '\0';

    int first = 1;

    for(relation* curr = relations->list; curr != NULL; curr = curr->next)
    {
        if(curr->hheap.binheap.count > 0)
        {
            if(curr->topval <=  0)
                rebuild_top(curr);

            output[0] = '\0';
            if(!first)
                strcat(output, " ");
            first = 0;
            strcat(output, "\"");
            strcat(output, curr->id_rel);
            strcat(output, "\" ");
            for(int j = 0; j < curr->top.count; j++)
            {
                strcat(output, "\"");
                strcat(output, curr->top.array[j]);
                strcat(output, "\" ");
            }
            sprintf(output + strlen(output), "%d;", curr->topval);
            fputs(output, stdout);
        }
    }
    if(output[0] == '\0')
        fputs("none\n", stdout);
    else
        fputc('\n', stdout);
}
