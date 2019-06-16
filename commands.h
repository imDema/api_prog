void addent(direct_ht ht, char* id_ent)
{
    //Calculate hash
    uint h = hash(id_ent);

    //TODO This is wrong, you need to allocate a relation instead and init it
    entity ent = new_entity();
    ht_insert(ht, strndup(id_ent, MAXLEN), ent, h);
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

    direct_ht ht_orig = ent_orig->ht_links;
    //Get the relation array for the link
    relarray rar = ht_search(ht_orig, id_dest, h_dest);
    if(rar == NULL)
    {
        //Initialize it if needed
        rar = new_relarray();
        ht_insert(ent_orig->ht_links, id_dest, rar, h_dest);
        ht_insert(ent_dest->ht_links, id_orig, rar, h_orig);
    }

    //Get relation info
    relation rel = create_relation(relations, id_rel);

    int direction = strcmp(id_orig, id_dest);
    int created = relarray_add(rar, rel->index, direction);

    //Set active relation arraylist to proper value
    if(created)
    {
        rel->active_count++;
        countarray_increase(&(ent_dest->in_counts), rel->index);
    }

    //TODO: Update max lists 
}

void delent(direct_ht ht, rel_db relations, char* id_ent) ///TODO HERE FIX WITH NEW DOUBLE RELATIONS
{
    //Go to the entry in the entity hashtable
    uint h_ent = hash(id_ent);
    entity ent = ht_search(ht, id_ent, h_ent);

    if(ent == NULL) return;

    //Update all outbound links relation counts and delete the links
    if(ent->ht_links != NULL)
    for(int i = 0; i < ent->ht_links->size; i++)
    {
        bucket bkt = ent->ht_links->buckets[i];
        if(bkt.hash != 0) //If a valid link has been found
        {
            entity dest = ht_search(ht, bkt.key, bkt.hash);

            //Remove all outbound relations
            relarray rar = bkt.value;
            if(rar->count > 0) //Decrease count for every active relation on link
            {
                int* active_indexes = relarray_get_active(rar);
                for(int j = 0; j < rar->count; j++)
                {
                    int index = active_indexes[j];
                    //DEBUG
                    if(dest->in_counts.array[index] < 1)
                        fprintf(stderr, "Something has gone very wrong, trying to reduce %s relation from %s below 0", id_ent, bkt.key);
                    //REMOVE
                    dest->in_counts.array[index]--;
                    relations->array[index].active_count--;
                }
                free(active_indexes);
            }

            free(rar);
            //TODO If memory needed free all inverse links from dest
        }
    }
    //Count orphaned relations and update relations active counts
    countarray counts = &(ent->in_counts);
    for(int j = 0, c = counts->count; j < counts->size && c > 0; j++)
    {
        int x = counts->array[j];
        if(x > 0)
        {
            relations->array[j].active_count -= x;
            relations->orphaned += x;
            c--;
        }
    }

    //Free the entity
    ht_free(ent->ht_links);
    free(ent->in_counts.array);
    free(ent);

    ht_delete(ht, id_ent, h_ent);
    

    //TODO reduce relation active counts
    //Iterate over links
    //Free link entries
    //Update top list
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
    relarray rar = ht_search(ent_orig->ht_links, id_dest, h_dest);
    
    //Skip if it does not exist
    if(rar == NULL) return;

    //Update arraylist entry if needed
    relation relitem = create_relation(relations, id_rel); //TODO REPLACE THIS

    int direction = strcmp(id_orig, id_dest);
    if(relarray_remove(rar, relitem->index, direction)) //If the relation existed remove it and update counts
    {
        relitem->active_count--;
        countarray_reduce(&(ent_dest->in_counts), relitem->index);
    }
    if(rar->count == 0) //Free the link
    {
        ht_delete(ent_orig->ht_links, id_dest, h_dest);
        relarray_free(rar);
    }

    //TODO: Update max lists
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

void report(hashtable rel_ht, hashtable link_ht, hashtable ent_ht)
{
    // rel_item* relations = malloc(rel_ht->count * sizeof(rel_item));

    // int k = 0, cnt = rel_ht->count;
    
    // //checkmask, remove if toplist active
    // int active = 0;
    // int* checkmask = malloc(cnt * sizeof(int));
    // topitem* tl = (topitem*)malloc(cnt * sizeof(topitem));
    // //checkmask
    // for(int i = 0; k < cnt; i++) //IF STUFF CRASHES IT'S BECAUSE cnt is wrong
    // {
    //     for (rel_item ri = (rel_item)rel_ht->buckets[i]; ri != NULL; ri = ri->next)
    //     {
    //         relations[k++] = ri;
    //         if(ri->active_count > 0)
    //             checkmask[active++] = ri->index;
    //     }
    // }
    
    // if(active > 0)
    // {
    //     for(int i = 0; i<cnt; i++)
    //         tl[i] = new_topitem(NULL,0);

    //     ///////TODO do not do this

    //     //Build top arrays
    //     gen_top(ent_ht, tl, checkmask, active);
        
    //     qsort(relations, cnt, sizeof(rel_item), comp_rel);
    //     //Print
    //     int size = 4;
    //     char** ar = malloc(size * sizeof(char*));
    //     for(int i = 0; i < cnt; i++)
    //     {
    //         if(relations[i]->active_count > 0)
    //         {
    //             int count = 0;
    //             printf("\"%s\" ", relations[i]->id_rel);

                
    //             for(topitem ti = tl[relations[i]->index]; ti != NULL; ti = ti->next)
    //             {
    //                 if(count == size)
    //                 {
    //                     size <<= 1;
    //                     ar = realloc(ar, size * sizeof(char*));
    //                 }
    //                 ar[count++] = ti->item->id_ent;
    //             }
    //             qsort(ar,count,sizeof(char*),cmpstr);
    //             for(int j = 0; j < count; j++)
    //             {
    //                 printf("\"%s\" ", ar[j]);
    //             }
    //             printf("%d;", tl[relations[i]->index]->val);
    //             if(cnt-i > 1) printf(" ");
    //         }
    //     }
    //     free(ar);
    //     printf("\n");
    // }
    // else
    //     printf("none\n");
    
    // free(checkmask);
    // for(int i=0; i<cnt; i++)
    //     free_topitem_list(tl[i]);
    // free(tl);
    // free(relations);
}
