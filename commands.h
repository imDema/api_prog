void addent(direct_ht ht, const char* id_ent)
{
    //Calculate hash
    uint h = hash(id_ent);

    //Check if duplicate
    if(ht_search(ht,id_ent, h) != NULL)
        return;
    entity ent = new_entity(id_ent);
    //Create new and insert in ht
    ht_insert(ht, ent->id_ent, ent, h);
}

void addrel(direct_ht ht, rel_db relations,
                const char* id_orig, const char* id_dest, const char* id_rel)
{
    //Check if both entities exist
    uint h_orig = hash(id_orig),
        h_dest = hash(id_dest);
    entity ent_orig = ht_search(ht, id_orig, h_orig),
        ent_dest = ht_search(ht, id_dest, h_dest);
    if(ent_orig == NULL || ent_dest == NULL)
        return;

    //Get the relation array for the link
    aa_node link = aa_search(ent_orig->tree_root, id_dest);
    relarray rar;

    if(link == NULL)
    {
        //Initialize it if needed
        rar = new_relarray();
        ent_orig->tree_root = aa_insert(ent_orig->tree_root, ent_dest->id_ent, rar);
        if(ent_orig != ent_dest)
            ent_dest->tree_root = aa_insert(ent_dest->tree_root, ent_orig->id_ent, rar);
    }
    else
        rar = link->rar;

    //Get relation info
    relation* rel = create_relation(relations, id_rel);

    int direction = strcmp(id_orig, id_dest);

    //Set active relation arraylist to proper value
    if(relarray_add(rar, rel->index, direction))
    {
        increase_relation_count(relations, rel->index, ent_dest->id_ent, h_dest);
    }
}

void dellinks(const direct_ht ht, const rel_db relations, aa_node tree, const char* id_ent)
{
    if(tree == NULL)
        return;
    
    dellinks(ht, relations, tree->left, id_ent);
    dellinks(ht, relations, tree->right, id_ent);

    uint h_ent2 = hash(tree->key);

    entity ent2 = ht_search(ht, tree->key, h_ent2);

    int m = tree->rar->size < relations->count ? tree->rar->size : relations->count;
    byte* ar = tree->rar->array;
    int order = strcmp(id_ent, ent2->id_ent);
    byte mask = order <= 0 ? FROM_FIRST : FROM_SECOND;
    for(int index = 0; index < m; index++)
    {
        byte b = ar[index];
        if(b & mask)
        {
            decrease_relations_count(relations, index, ent2->id_ent, h_ent2);
        }
    }

    relarray_free(tree->rar);
    ent2->tree_root = aa_delete(ent2->tree_root, id_ent);
    if(order != 0)
        free(tree);
}

void delent(direct_ht ht, rel_db relations, const char* id_ent)
{
    //Go to the entry in the entity hashtable
    uint h_ent = hash(id_ent);
    entity ent = ht_search(ht, id_ent, h_ent);

    if(ent == NULL) return;

    //Update all outbound links relation counts and delete the links
    dellinks(ht, relations, ent->tree_root, ent->id_ent);

    for(int index = 0, m = relations->count; index < m; index++)
        delete_relation_count(relations, index, id_ent, h_ent); 

    ht_delete(ht, id_ent, h_ent);
    free(ent->id_ent);
    free(ent);
}

void delrel(direct_ht ht, rel_db relations, const char* id_orig, const char* id_dest, const char* id_rel)
{
    //Calculate hashes and verify existence
    uint h_orig = hash(id_orig),
        h_dest = hash(id_dest);
    entity ent_orig = ht_search(ht, id_orig, h_orig),
        ent_dest = ht_search(ht, id_dest, h_dest);
    if(ent_orig == NULL || ent_dest == NULL)
        return;
    

    //Search link
    aa_node link = aa_search(ent_orig->tree_root, id_dest);

    if(link == NULL) return;

    relarray rar = link->rar;
    
    //Skip if it does not exist
    if(!rar) return;

    //Update arraylist entry if needed
    relation* rel = ht_search(relations->ht, id_rel, hash(id_rel));
    if(!rel) return;

    int direction = strcmp(id_orig, id_dest);
    if(relarray_remove(rar, rel->index, direction)) //If the relation existed remove it and update counts
    {
        decrease_relations_count(relations, rel->index, id_dest, h_dest);
    }
    if(rar->count == 0) //Free the link
    {
        ent_orig->tree_root = aa_delete(ent_orig->tree_root, id_dest);
        if(ent_orig != ent_dest)
            ent_dest->tree_root = aa_delete(ent_dest->tree_root, id_orig);

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
    char output[1024];
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
