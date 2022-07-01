typedef struct Myhash_node
{
    char* IP;
    char* DomainName;
    long long TimeStamp;
    struct Myhash_node* Next;
    struct Myhash_node* Previous;
} HashNode;
typedef struct MyHash_Linked_List
{
    HashNode* Head;
    HashNode* Tail;
}  HashLinkedList;
HashLinkedList * GetEmptyHashList();
void HashAddLast(HashLinkedList * List, char * IP, char* DomainName);
void HashAddFirst(HashLinkedList * List, char * IP, char* DomainName);
HashNode* HashRemoveFirst(HashLinkedList* List);
HashNode* HashRemoveLast(HashLinkedList* List);
void HashprintList(HashLinkedList* List);
