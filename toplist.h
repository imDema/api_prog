#include<stdlib.h>
struct toplist
{
    char* id_ent;
    struct toplist* next;
};
typedef struct toplist* Toplist;

