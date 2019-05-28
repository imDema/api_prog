#include <stdlib.h>

struct node
{
    int complete;
    struct node** refs[4];
};
typedef struct node* Node;

#define OFFSET_UPPER -65 // -'A';
#define OFFSET_NUMBER -22 // -'0' + 'z'-'a' + 1
#define OFFSET_LOWER -61 //- 'a' + 'Z' - 'A' + '9' - '0' + 2;
#define UNDERSCORE 62
#define DASH 63

int hash(char c)
{
    if(c >= 'A' && c <= 'Z')
        return c + OFFSET_UPPER;
    
    else if (c >= 'a' && c <= 'z')
        return c + OFFSET_LOWER;

    else if(c >= '0' && c <= '9')
        return c + OFFSET_NUMBER;
    
    else if(c == '_')
        return UNDERSCORE;
    
    else 
        return DASH;
}

void insert(Node root, char* word)
{
    if(word[0] == '\0' || word[0] == '\n')
    {
        root->complete = 1;
    }
    else
    {
        int k = hash(word[0]);
    }
    
}