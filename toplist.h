#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef struct _toparray
{
    char** array;
    int count;
    int size;
} toparray;

struct _node
{
    struct _node* next;
    char* id;
    int val;
};
typedef struct _node* node;

typedef struct toplist
{
    int min_val;
    node head;
    int count;
    int max_trigger;
    int rebuild;
} toplist;

node new_node(char* key, int val)
{
    node new = malloc(sizeof(struct _node));
    new->next = NULL;
    new->id = key;
    new->val = val;
    return new;
}

void tl_insert(toplist* tl, char* key, int val)
{
    if(tl->rebuild) return;
    if(tl->count == 0)
    {
        tl->min_val = val;
        tl->head = new_node(key,val);
        tl->count = 1;
    }
    else if(tl->count < tl->max_trigger)
    {
        if(val < tl->min_val)
        {
            tl->min_val = val;
            node new = new_node(key,val);
            new->next = tl->head;
            tl->head = new;
        }
        else
        {
            node curr = tl->head;
            while(curr->next != NULL && curr->next->val <= val)
            {
                curr = curr->next;
            }
            node new = new_node(key,val);
            new->next = curr->next;
            curr->next = new;
        }
        tl->count++;
    }
    else
    {
        while(tl->head != NULL && tl->head->val == tl->min_val)
        {
            node temp = tl->head;
            tl->head = tl->head->next;
            free(temp);
            tl->count--;
        }
        if(tl->head == NULL)
            tl->rebuild = 1;
        else
        {
            tl_insert(tl, key, val);
        }
    }
}

void tl_remove(toplist* tl, char* key, int val)
{
    if(val < tl->min_val || tl->rebuild) return;

    if(!strcmp(key, tl->head->id))
    {
        node temp = tl->head;
        tl->head = tl->head->next;
        free(temp);
        tl->count--;

        if(tl->head == NULL)
        {
            tl->rebuild = 1;
            return;
        }

        if(tl->head->val < tl->min_val)
            tl->min_val = tl->head->val;
        return;
    }

    node curr = tl->head;
    while (curr->next != NULL && strcmp(key, curr->next->id))
    {
        curr = curr->next;
    }
    if(curr->next == NULL)
        return;
    else
    {
        node temp = curr->next;
        curr->next = curr->next->next;
        free(temp);
    }
}