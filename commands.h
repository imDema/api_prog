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

void delrel(hashtable rel_ht, hashtable link_ht, hashtable ent_ht, char* id_orig, char* id_dest, char* id_rel)
{
    //Calculate combined hash
    char* uid = uidof(id_orig, id_dest);
    int index = hash(uid) % link_ht->size;
    

    //Search entry in link hashtable
    link_item link;
    for(link = link_ht->buckets[index]; link != NULL; link = link->next)
    {
        if(!strcmp(uid, link->uid))
            break;
    }
    free(uid);
    
    //Skip if it does not exist
    if(link == NULL) return;

    //Update arraylist entry if needed
    byte mask = strcmp(id_orig,id_dest) < 0 ? FROM_FIRST : FROM_SECOND;

    ent_item ent_dest = (ent_item)ht_ent_search(ent_ht,id_dest);
    if(ent_dest == NULL) fprintf(stderr, "Error deleting relation, destination entity does not exist\n");

    if(!relarray_remove(link->relations, index, mask)) return;
    
    countarray_reduce(ent_dest->relcounts, index);

    ht_rel_search(rel_ht, id_rel)->active_count--;

    //if(link->relations->count == 0) ht_link_free() TODO free link

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

int cmpstr(const void* a, const void* b)
{
    const char* aa = *(const char**)a;
    const char* bb = *(const char**)b;
    return strcmp(aa,bb);
}

void report(hashtable rel_ht, hashtable link_ht, hashtable ent_ht)
{
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
        gen_top(ent_ht, tl, checkmask, active);
        
        qsort(relations, cnt, sizeof(rel_item), comp_rel);
        //Print
        int size = 4;
        char** ar = malloc(size * sizeof(char*));
        for(int i = 0; i < cnt; i++)
        {
            if(relations[i]->active_count > 0)
            {
                int count = 0;
                printf("\"%s\" ", relations[i]->id_rel);

                
                for(topitem ti = tl[relations[i]->index]; ti != NULL; ti = ti->next)
                {
                    if(count == size)
                    {
                        size <<= 1;
                        ar = realloc(ar, size * sizeof(char*));
                    }
                    ar[count++] = ti->item->id_ent;
                }
                qsort(ar,count,sizeof(char*),cmpstr);
                for(int j = 0; j < count; j++)
                {
                    printf("\"%s\" ", ar[j]);
                }
                printf("%d;", tl[relations[i]->index]->val);
                if(cnt-i > 1) printf(" ");
            }
        }
        free(ar);
        printf("\n");
    }
    else
        printf("none\n");
    
    free(checkmask);
    for(int i=0; i<cnt; i++)
        free_topitem_list(tl[i]);
    free(tl);
    free(relations);
}
