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

void addrel(char* id_orig, char* id_dest, char* id_rel)
{
    //Check relation list in rel_ht
        //If it doesn't create a new entry and increment nonce
    //Calculate combined hash
    //Search entry in link hashtable
        //Create entry if it does not exist
    //Set active relation arraylist to proper value (update count)
    //Add link pointer to both entities entries list
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
