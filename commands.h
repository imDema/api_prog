#define FROM_FIRST 0x1
#define FROM_SECOND 0x2

void addent(direct_ht ht, char* id_ent)
{
    //Calculate hash
    uint h = hash(id_ent);
    direct_ht links = calloc(1, sizeof(struct _direct_ht));
    ht_insert(ht, strndup(id_ent, MAXLEN), links, h);
}

void addrel(direct_ht ht, hashtable rel_ht,
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
        ent_orig->ht_links = new_direct_ht(DIRECT_HT_DEFAULT_SIZE);

    direct_ht ht_orig = ent_orig->ht_links;
    //Get the relation array for the link
    relarray rar = ht_search(ht_orig, id_dest, h_dest);
    if(rar == NULL)
    {
        //Initialize it if needed
        rar = new_relarray();
        ht_insert(ent_orig->ht_links, id_dest, rar, h_dest);
    }

    //Get relation index        //TODO replace this
    rel_item relitem = create_relation(rel_ht, id_rel);

    int created = relarray_add(rar, relitem->index);

    //Set active relation arraylist to proper value
    if(created)
    {
        relitem->active_count++;
        countarray_increase(&(ent_dest->in_counts), relitem->index);
    }

    //TODO: Update max lists 
}

void delent(hashtable ent_ht, hashtable link_ht, char* id_ent)
{
    //Go to the entry in the entity hashtable
    ent_item entity = ht_ent_search(ent_ht, id_ent);

    if(entity == NULL) return;

    for(ll_node ll_link = entity->links; ll_link != NULL; ll_link = ll_link->next)
    {
        link_item lnk = ll_link->link;

        //TODO reduce relation active counts

        if(lnk->prev == NULL)
        {
            lnk->next->prev = NULL;
            uint index = hash(lnk->uid) % link_ht->size;
            link_ht->buckets[index] = lnk->next;
            free_link_item(link_ht, lnk);
        }
        else
        {
            lnk->prev->next = lnk->next;
            if(lnk->next != NULL) lnk->next->prev = lnk->prev;
            free_link_item(link_ht, lnk);
        }
    }
    //Iterate over links
    //Free link entries
    //Update top list
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

    if (link->relations->count == 0) //Free link
    {
        if(link->prev == NULL)
        {
            link->next->prev = NULL;
            link_ht->buckets[index] = link->next;
            free_link_item(link_ht, link);
        }
        else
        {
            link->prev->next = link->next;
            if(link->next != NULL) link->next->prev = link->prev;
            free_link_item(link_ht, link);
        }
    }
    
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
