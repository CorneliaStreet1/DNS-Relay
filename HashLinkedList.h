typedef struct node
{
    char* IP;
    char* DomainName;
    long long TimeStamp;
    struct node* Next;
    struct node* Previous;
} HashNode;
typedef struct Hash_Linked_List
{
    HashNode* Head;
    HashNode* Tail;
}  HashLinkedList;
HashLinkedList * GetEmptyList();
void AddLast(HashLinkedList * List, char * IP, char* DomainName);
void AddFirst(HashLinkedList * List, char * IP, char* DomainName);
HashNode* RemoveFirst(HashLinkedList* List);
HashNode* RemoveLast(HashLinkedList* List);
void printList(HashLinkedList* List);
