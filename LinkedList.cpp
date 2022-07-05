#include "LinkedList.h"
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
LinkedList * GetEmptyList() {
    LinkedList* list =(LinkedList*) malloc(sizeof(LinkedList));
    list->Size = 0;
    list->Head =(Node*) malloc(sizeof(Node));
    list->Tail =(Node*) malloc(sizeof(Node));
    list->Head->DomainName = NULL;
    list->Head->IP = NULL;
    list->Head->Next = list->Tail;
    list->Head->Previous = NULL;
    list->Tail->DomainName = NULL;
    list->Tail->IP = NULL;
    list->Tail->Next = NULL;
    list->Tail->Previous = list->Head;
    return list;
}
void AddLast(LinkedList * List, char * IP, char* DomainName) {
    Node* OldLast = List->Tail->Previous;
    Node* newLast =(Node*) malloc(sizeof(Node));
    newLast->DomainName = DomainName;
    newLast->IP = IP;
    OldLast->Next = newLast;
    newLast->Previous = OldLast;
    newLast->Next = List->Tail;
    List->Tail->Previous = newLast; 
    List->Size += 1;
}
void AddFirst(LinkedList* List,char * IP, char* DomainName) {
    printf("addFirst begin\n");
    Node* OldFirst = List->Head->Next;
    Node* newFirst =(Node*) malloc(sizeof(Node));
    newFirst->DomainName = DomainName;
    newFirst->IP = IP;
    newFirst->Next = OldFirst;
    newFirst->Previous = List->Head;
    OldFirst->Previous = newFirst;
    List->Head->Next = newFirst;
    List->Size += 1;
    printf("addFirst end\n");
}
Node* RemoveFirst(LinkedList* List) {
    Node* FirstNode = List->Head->Next;
    Node* returnNode =(Node*) malloc(sizeof(Node));
    List->Head->Next = FirstNode->Next;
    FirstNode->Next->Previous = List->Head;
    returnNode->DomainName = FirstNode->DomainName;
    returnNode->IP = FirstNode->IP;
    free(FirstNode);
    List->Size -= 1;
    return returnNode;
}
Node* RemoveLast(LinkedList* List) {
    printf("removeLast begin\n");
    Node* LastNode = List->Tail->Previous;
    printf("LastNode.domainName:%s\nLastNode.IP:%s\n", LastNode->DomainName, LastNode->IP);
    if (LastNode->DomainName == NULL) {
        printf("LastNode ptr:%p\n", LastNode);
        printf("List head ptr:%p\nList tail ptr:%p\n", List->Head, List->Tail);
    }
    Node* returnNode =(Node*) malloc(sizeof(Node));
    returnNode->DomainName = LastNode->DomainName;
    returnNode->IP = LastNode->IP;
    LastNode->Previous->Next = List->Tail;
    List->Tail->Previous = LastNode->Previous;
    free(LastNode);//直接返回returnNode，断开连接不就行了？
    List->Size -= 1;
    printf("removeLast end\n");
    return returnNode;
}
int isEmpty(LinkedList* List) {
    return List->Size == 0;
}
int Size(LinkedList* List) {
    return List->Size;
}
void printList(LinkedList* List) {
    Node* p = List->Head->Next;
    while (p != List->Tail){
        printf("IP: ");
        printf(p->IP);
        printf("  DomainName:");
        printf(p->DomainName);
        printf("\n");
        p = p->Next;
    }
}
Node* RemoveNode(LinkedList* List, char* DomainName) {
    Node* p = List->Head->Next;
    while (p != List->Tail)
    {
        if (p != NULL && strcmp(p, DomainName) == 0)
        {
            Node
            Node* nextNode = p->Next;
            Node* PreviousNode = p->Previous;
            next->Previous = PreviousNode;
            PreviousNode->Next = nextNode;
            printf("In RemoveNode(), Removed:");
            printf(DomainName);
            printf("\n");
            free(p);
            p = nextNode;
        }
        else {
            p = p->Next
        }
        
    }
    
}
Node* get(LinkedList* List, int index) {
    Node* p = List->Head->Next;
    int i = 0;
    while (i != index)
    {
        i ++;
        p = p->Next;
    }
    return p;
}