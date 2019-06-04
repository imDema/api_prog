#include <stdlib.h>

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

Node new_node()
{
    Node n = (Node) calloc(1 , sizeof(struct node));
    return n;
}

