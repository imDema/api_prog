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