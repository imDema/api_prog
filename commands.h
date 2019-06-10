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

link_item create_link(hashtable link_ht, ent_item ent1, ent_item ent2)
{
    //Calculate combined hash
    char* uid = uidof(ent1->id_ent, ent2->id_ent);

    uint h = hash(uid);
    //Search entry in link hashtable
    int index = h % link_ht->size;

    link_item link = NULL;
    link_item found = (link_item) link_ht->buckets[index];

    //If the bucket is not empty
    if(found != NULL)
    {
        //Look for item in bucket with the uid we are looking for
        if(!strcmp(uid, found->id_link))
            link = found;
        else while (found->next != NULL)
        {
            found = found->next;
            if(!strcmp(uid, found->id_link))
            {
                link = found;
                break;
            }
        }
        //If link is still NULL, we havent found it, so we create a new link item and place it in the list
        if(link == NULL)
        {
            link = new_linkitem(uid);
            found->next = link;

            //Add link pointer to both entities entries list
            ll_insert(ent1->links, link);
            ll_insert(ent2->links, link);
        }
    }
    //The bucket is empty, so we create a new link item and point the bucket to it
    else
    {
        link = new_linkitem(uid);
        link_ht->buckets[index] = link;
        
        //Add link pointer to both entities entries list
        ll_insert(ent1->links, link);
        ll_insert(ent2->links, link);
    }
    return link;
}

void addrel(hashtable ent_ht, hashtable link_ht, hashtable rel_ht,
                char* id_orig, char* id_dest, char* id_rel)
{
    //Check if both entities exist
    ent_item ent1 = ht_ent_search(ent_ht,id_orig), ent2 = ht_ent_search(ent_ht,id_dest);
    if(ent1 == NULL || ent2 == NULL)
        return;

    
    rel_item relitem = create_relation(rel_ht, id_rel);
    
    link_item link = create_link(link_ht, ent1, ent2);

    //At this point 'link' holds the node of the corrisponding link, either an existing one or a new one
    byte mask = strcmp(id_orig,id_dest) < 0 ? FROM_FIRST : FROM_SECOND;
    int created = relarray_add(link->relations, relitem->index, mask);

    //Set active relation arraylist to proper value (update count)
    if(created)
        relitem->active_count++;

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

void report()
{
    //Sort relation entries
        //qsort()
    //If count == 0 skip
    //Query max lists to get top element
    //Print
}
