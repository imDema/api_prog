#define FROM_FIRST 0x1
#define FROM_SECOND 0x2

void addent(hashtable ent_ht, char* id_ent)
{
    //Calculate hash
    uint h = hash(id_ent);
    //Search entry in entity ht
    int index = h % ent_ht->size;
    
    //Return if item exists, else get last chain item (or NULL if empty)
    ent_item item = (ent_item)ent_ht->buckets[index];
    if(item != NULL)
    {
        if(!strcmp(item->id_ent, id_ent)) return;
        while(item->next != NULL)
        {
            if(!strcmp(item->next->id_ent, id_ent))
                return;
            item = item->next;
        }
    }

    ent_ht->count++;
    //Add if it doesn't exist
    ent_item newent = new_ent_item(id_ent);
    if(item != NULL)
    {
        item->next = newent;
    }
    else
    {
        ent_ht->buckets[index] = newent;
    }
}

void delent(char* id_ent)
{
    //Go to the entry in the entity hashtable
    //Iterate over links
    //Free link entries
    //Update top list
}

void addrel(hashtable ent_ht, hashtable link_ht, hashtable rel_ht,
                char* id_orig, char* id_dest, char* id_rel)
{
    //Check if both entities exist
    ent_item ent_orig = ht_ent_search(ent_ht,id_orig), ent_dest = ht_ent_search(ent_ht,id_dest);
    if(ent_orig == NULL || ent_dest == NULL)
        return;

    
    rel_item relitem = create_relation(rel_ht, id_rel);
    
    link_item link = create_link(link_ht, ent_orig, ent_dest);

    //At this point 'link' holds the node of the corrisponding link, either an existing one or a new one
    byte mask = strcmp(id_orig,id_dest) < 0 ? FROM_FIRST : FROM_SECOND;
    //TODO CHECK IF CREATED WORKS PROPERLY
    int created = relarray_add(link->relations, relitem->index, mask);

    //Set active relation arraylist to proper value TODO! Update entity relarray count
    if(created)
    {
        relitem->active_count++;
        countarray_increase(ent_dest->relcounts, relitem->index);
    }

    //TODO: Update max lists 
}

void delrel(char* id_orig, char* id_dest, char* id_rel)
{
    //Calculate combined hash
    //Search entry in link hashtable
        //Skip if it does not exist
    //Update arraylist entry if needed
    //Reduce count if arraylist entry becomes 0
    //If count is 0 delete link entry
    //TODO: Update max lists
}

int comp_rel(const void* r1, const void* r2)
{
    char* id1 = (*(rel_item*)r1)->id_rel;
    char* id2 = (*(rel_item*)r2)->id_rel;
    return strcmp(id1,id2);
}

void report(hashtable rel_ht, hashtable link_ht, hashtable ent_ht)
{
    //Sort relation entries
    rel_item* relations = malloc(rel_ht->count * sizeof(rel_item));

    int k = 0, cnt = rel_ht->count;
    
    //checkmask, remove if toplist active
    int active = 0;
    int* checkmask = malloc(cnt * sizeof(int));
    topitem* tl = (topitem*)malloc(cnt * sizeof(topitem));
    //checkmask
    for(int i = 0; k < cnt; i++) //IF STUFF CRASHES IT'S BECAUSE cnt is wrong
    {
        for (rel_item ri = (rel_item)rel_ht->buckets[i]; ri != NULL; ri = ri->next)
        {
            relations[k++] = ri;
            if(ri->active_count > 0)
                checkmask[active++] = ri->index;
        }
    }
    
    if(active > 0)
    {
        for(int i = 0; i<cnt; i++)
            tl[i] = new_topitem(NULL,0);

        ///////TODO do not do this

        //Build top arrays
        
        //Iterate over all ht entities
        for(int i = 0, left = ent_ht->count; left > 0; i++) //IF STUFF CRASHES IT'S BECAUSE ht->count is wrong
        {
            for(ent_item entity = ent_ht->buckets[i]; entity != NULL; entity = entity->next)
            {
                left--;
                //for each active relation index
                for(int j = 0; j<active; j++)
                {
                    int index = checkmask[j];
                    
                    //if the current entity entering relation count is higher or equal to the top found
                    int x;
                    if(entity->relcounts->size > index && (x = entity->relcounts->array[index]) > 0 && x >= tl[index]->val) //Possible error here
                    {
                        //If equal enqueue
                        if(x == tl[index]->val)
                        {
                            topitem newitem = new_topitem(entity, x);
                            topitem k = tl[index];
                            //Proceed until k is the greatest node smaller than new one
                            while(k->next != NULL && strcmp(newitem->item->id_ent, k->item->id_ent) > 0)
                                k = k->next;
                            //Stitch new node between k and k->next
                            newitem->next = k->next;
                            k->next = newitem;
                        }
                        //If greater
                        else
                        {
                            //Free the current list
                            free_topitem_list(tl[index]);
                            //Put new one on top of the list
                            topitem newitem = new_topitem(entity, x);
                            tl[index] = newitem;
                        }
                    }
                }
            }
        }
        qsort(relations, cnt, sizeof(rel_item), comp_rel);
        //Print
        for(int i = 0; i < cnt; i++)
        {
            if(relations[i]->active_count > 0)
            {
                printf("\"%s\" ", relations[i]->id_rel);
                for(topitem ti = tl[relations[i]->index]; ti != NULL; ti = ti->next)
                {
                    printf("\"%s\" ", ti->item->id_ent);
                }
                printf("%d; ", tl[relations[i]->index]->val);
            }
        }
        printf("\n");
    }
    else
        printf("none\n");
    
    free(checkmask);
    for(int i=0; i<cnt; i++)
        free_topitem_list(tl[i]);
    free(tl);
}
