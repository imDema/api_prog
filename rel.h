#include<stdlib.h>
#include "structures.h"
#include "toplist.h"

struct relations
{
    char* rel_id;
    Node links;
    Toplist tops;
    int max = 0;
};

typedef struct relations* Relations;

Relations init_relations(char* rel_id)
{
    struct relations rel;
    rel.links = new_node();
    return &rel;
}