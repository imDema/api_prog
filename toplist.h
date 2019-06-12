struct _topitem
{
    struct _topitem* next;
    ent_item item;
    int val;
};
typedef struct _topitem* topitem;

topitem new_topitem(ent_item ent, int val)
{
    topitem it = malloc(sizeof(struct _topitem));
    it->next = NULL;
    it->item = ent;
    it->val = val;
    return it;
}

void free_topitem_list(topitem ti)
{
    if(ti == NULL)
        return;
    if(ti->next != NULL)
        free_topitem_list(ti->next);
    free(ti);
}

void gen_top(hashtable ent_ht, topitem* tl, int* checkmask, int n)
{
    //Iterate over all ht entities
    for(int i = 0, left = ent_ht->count; left > 0; i++) //IF STUFF CRASHES IT'S BECAUSE ht->count is wrong
        for(ent_item entity = ent_ht->buckets[i]; entity != NULL; entity = entity->next)
        {
            left--;
            //for each active relation index
            for(int j = 0; j<n; j++)
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
                        newitem->next = tl[index];
                        tl[index] = newitem;
                    }
                    //If greater
                    else
                    {
                        //Free the current list
                        free_topitem_list(tl[index]);
                        //Put new one on top of the list
                        tl[index] = new_topitem(entity,x);
                    }
                }
            }
        }
}