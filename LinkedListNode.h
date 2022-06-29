typedef struct node
{
    char* IP;
    char* DomainName;
    struct node* Next;
    struct node* Previous;
} Node;
