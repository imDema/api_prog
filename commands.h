void addent(direct_ht ht, const char* id_ent)
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

    //Find the link to get to the relarray
    aa_node link = aa_search(ent_orig->tree_root, id_dest);
    relarray rar;

    if(link == NULL) //If the link didn't exist create a new one
    {
        //Create a new relarray and place it in both the entities trees
        rar = new_relarray();
        ent_orig->tree_root = aa_insert(ent_orig->tree_root, ent_dest->id_ent, rar);
        //Check if the relation is reflessive, if not don't add it twice
        if(ent_orig != ent_dest)
            ent_dest->tree_root = aa_insert(ent_dest->tree_root, ent_orig->id_ent, rar);
    }
    else //If the link existed retrieve relation array
        rar = link->rar;

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

void dellinks(direct_ht ht, rel_db relations, aa_node tree, const char* id_ent)
{
    //Recursion stop
    if(tree->left != NULL)
        dellinks(ht, relations, tree->left, id_ent);
    if(tree->right != NULL)
        dellinks(ht, relations, tree->right, id_ent);

    if(tree->key == id_ent) //If reflessive node
    {
        relarray_free(tree->rar);
        free(tree);
        return;
    }

    //Locate the linked entity
    uint h_ent2 = hash(tree->key);
    entity ent2 = ht_search(ht, tree->key, h_ent2);
    if(ent2 == NULL)
        fputs("!!!\n", stderr);

    //Iterate over all relation indexes and remove all active relation
    int order = strcmp(id_ent, tree->key);
    
    byte* ar = tree->rar->array;
    byte mask = order <= 0 ? FROM_FIRST : FROM_SECOND;

    int m = tree->rar->size < relations->count ? tree->rar->size : relations->count;
    for(int index = 0; index < m; index++)
    {
        byte b = ar[index];
        if(b & mask)
        {
            //If an active relation on the link is entering the linked node decrease the counts
            decrease_relations_count(relations, index, ent2->id_ent, h_ent2);
        }
    }

    //free the relarray
    relarray_free(tree->rar);
    free(tree);

    //Remove the link from the second entity tree
    ent2->tree_root = aa_delete(ent2->tree_root, id_ent);
}

void delent(direct_ht ht, rel_db relations, const char* id_ent)
{
    //Go to the entry in the entity hashtable
    uint h_ent = hash(id_ent);
    entity ent = ht_search(ht, id_ent, h_ent);

    if(ent == NULL) return;

    //Delete all links updating the heap in the meantime
    if(ent->tree_root != NULL)
        dellinks(ht, relations, ent->tree_root, ent->id_ent);

    //Update all the relation heaps for the entity deletion
    for(int index = 0, m = relations->count; index < m; index++)
        delete_relation_count(relations, index, id_ent, h_ent);

    //Delete the entity
    ht_delete(ht, id_ent, h_ent);
    //Free the resources
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

    //Get the relarray
    relarray rar = link->rar;
    if(!rar) return;

    //Find the relation
    relation* rel = ht_search(relations->ht, id_rel, hash(id_rel));
    if(!rel) return;

    int direction = strcmp(id_orig, id_dest);
    if(relarray_remove(rar, rel->index, direction))
    {
        //If the relation existed decrease the entry in the heap
        decrease_relations_count(relations, rel->index, id_dest, h_dest);
    }
    if(rar->count == 0) //If there are no more active relations on the link delete it
    {
        //Delete the link from the origin
        ent_orig->tree_root = aa_delete(ent_orig->tree_root, id_dest);
        //If the relation is not reflessive delete the link from the destination
        if(ent_orig != ent_dest)
            ent_dest->tree_root = aa_delete(ent_dest->tree_root, id_orig);

        relarray_free(rar);
    }
}

void fast_int_format(char* string, int integer)
{
    char formatted[10]; //MAX_INT ha 10 cifre
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
    rel->topval = hh_peek(&(rel->hheap));
    arraylist_reset(&(rel->top));
    hh_addto_topar(&(rel->hheap), &(rel->top), rel->topval, 0);
    qsort(rel->top.array, rel->top.count, sizeof(char*), cmpstr);
}

void report(rel_db relations)
{
    //Memory buffer
    char output[1024];
    output[0] = '\0';

    int first = 1;

    //Scroll through the ordered list
    for(relation* curr = relations->list; curr != NULL; curr = curr->next)
    {
        //Only print if there are relations
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
            fast_int_format(output + strlen(output), curr->topval);
            strcat(output, ";");
            //sprintf(output + strlen(output), "%d;", curr->topval);
            fputs(output, stdout);
        }
    }
    if(output[0] == '\0')
        fputs("none\n", stdout);
    else
        fputc('\n', stdout);
}
