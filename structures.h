#include <stdlib.h>

#define OFFSET_UPPER -65 // -'A';
#define OFFSET_NUMBER -22 // -'0' + 'z'-'a' + 1
#define OFFSET_LOWER -61 //- 'a' + 'Z' - 'A' + '9' - '0' + 2;
#define UNDERSCORE 62
#define DASH 63

struct node
{
    int complete;
    struct node** refs[4];
    int inbound;
};
typedef struct node* Node;

int hashchar(char c);
Node new_node();
void insert(Node root, char* word)
{
    if(word[0] == '\0' || word[0] == '\n')
    {
        root->complete = 1;
    }
    else
    {
        int k = hashchar(word[0]);
        int div = k/4;
        int mod = k%16;
        if(root->refs[div] == NULL)
        {
            root->refs[div] = (Node *)calloc(16,sizeof(Node));
        }
        if(root->refs[div][mod] == NULL)
        {
            root->refs[div][mod] = new_node();
        }
        insert(root->refs[div][mod], word+1);
    }
    
}

int hashchar(char c)
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

Node new_node()
{
    Node n = (Node) calloc(1 , sizeof(struct node));
    return n;
}

