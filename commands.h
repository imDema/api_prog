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
    relation rel = create_relation(relations, id_rel);

    int direction = strcmp(id_orig, id_dest);

    //Set active relation arraylist to proper value
    if(relarray_add(rar, rel->index, direction))
    {
        rel->active_count++;
        countarray_increase(&(ent_dest->in_counts), rel->index);
    }

    //TODO: Update top lists 
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
            if(b > 0)
            {
                if(b & mask)
                {
                    dest->in_counts.array[index]--;
                }
                relations->array[index].active_count -= b == 0x11 ? 2 : 1;
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
    if(ent_ht != NULL && ent_ht->count > 0)
    {
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
    //Free the entity
    free(ent->in_counts.array);
    free(ent);

    ht_delete(ht, id_ent, h_ent);
    //TODO Update top list
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
    if(rar == NULL) return;

    //Update arraylist entry if needed
    int* ind = ht_search(relations->ht, id_rel, hash(id_rel));
    if(ind == NULL) return;
    
    relation relitem = &(relations->array[*ind]);

    int direction = strcmp(id_orig, id_dest);
    if(relarray_remove(rar, relitem->index, direction)) //If the relation existed remove it and update counts
    {
        relitem->active_count--;
        countarray_reduce(&(ent_dest->in_counts), relitem->index);
    }
    if(rar->count == 0) //Free the link
    {
        ht_delete(ent_orig->ht_links, id_dest, h_dest);
        ht_delete(ent_dest->ht_links, id_orig, h_orig);
        relarray_free(rar);
    }

    //TODO: Update top lists
}

int comp_rel(const void* r1, const void* r2)
{
    char* id1 = (*(rel_item*)r1)->id_rel;
    char* id2 = (*(rel_item*)r2)->id_rel;
    return strcmp(id1,id2);
}

int cmpstr(const void* a, const void* b)
{
    const char* aa = *(const char**)a;
    const char* bb = *(const char**)b;
    return strcmp(aa,bb);
}

int cmptopar(const void* a, const void* b)
{
    const toparray aa = *(toparray*)a;
    const toparray bb = *(toparray*)b;
    return strcmp(aa.id_rel, bb.id_rel);
}

void report(direct_ht ht, rel_db relations)
{
    int M = relations->count;
    int none = 1;
    for(int i = 0; i < M; i++)
    {
        if(relations->array[i].active_count > 0)
        {
            none = 0;
            break;
        }
    }
    if(none)
    {
        fputs("none\n", stdout);
        return;
    }

    toparray* maxlist = calloc(M, sizeof(toparray));

    //PASS 1: Find max values
    //Go over all buckets
    const int size = ht->size;
    bucket* buckets = ht->buckets;
    for(int i = 0; i < size; i++)
    {
        bucket bkt = buckets[i];
        if(bkt.hash != 0x0)
        {
            //Go over the count for entering relations
            countarray cnt = ((entity)bkt.value)->in_counts;
            for(int index = 0, max = cnt.size < M ? cnt.size : M; index < max; index++)
            {
                if(cnt.array[index] > maxlist[index].value)
                    maxlist[index].value = cnt.array[index];
            }
        }
    }
    //PASS 2: Build lists
    for(int i = 0; i < size; i++)
    {
        bucket bkt = buckets[i];
        if(bkt.hash != 0x0)
        {
            //Go over the count for entering relations
            countarray cnt = ((entity)bkt.value)->in_counts;
            for(int index = 0, max = cnt.size < M ? cnt.size : M; index < max; index++)
            {
                int x = cnt.array[index];
                if(maxlist[index].value > 0 && x == maxlist[index].value) //Add to the list
                    arralylist_push(&maxlist[index], bkt.key);

            }
        }
    }
    for(int i = 0; i < M; i++)
    {
        qsort(maxlist[i].array, maxlist[i].count, sizeof(char*), cmpstr);
        maxlist[i].id_rel = relations->array[i].id_rel;
    }
    
    int first = 1;
    qsort(maxlist, M, sizeof(toparray), cmptopar);
    for(int i = 0; i < M; i++)
    {
        toparray topar = maxlist[i];
        if(topar.value > 0)
        {
            if(!first)
                fputc(' ', stdout);
            first = 0;
            fputc('\"', stdout);
            fputs(topar.id_rel, stdout);
            fputs("\" ", stdout);
            for(int j = 0; j < topar.count; j++)
            {
                fputc('\"', stdout);
                fputs(topar.array[j], stdout);
                fputs("\" ", stdout);
            }
            printf("%d;", topar.value);
        }
        free(topar.array);
    }
    fputc('\n', stdout);
    free(maxlist);
}
