#include "HashLinkedList.h"
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
HashLinkedList * GetEmptyHashList() {
    HashLinkedList* List = (HashLinkedList*)malloc(sizeof(HashLinkedList));
    List->Head = (HashNode*)malloc(sizeof(HashNode));
    List->Tail = (HashNode*)malloc(sizeof(HashNode));
    List->Head->DomainName = NULL;
    List->Head->IP = NULL;
    List->Head->Next = List->Tail;
    List->Head->Previous = NULL;
    List->Head->TimeStamp = 0;
    List->Tail->DomainName = NULL;
    List->Tail->IP = NULL;
    List->Tail->Next = NULL;
    List->Tail->Previous = List->Head;
    List->Tail->TimeStamp = 0;
    return List;
}
void HashAddLast(HashLinkedList * List, char * IP, char* DomainName) {
    HashNode* newLast =(HashNode*) malloc(sizeof(HashNode));
    HashNode* OldLast =(HashNode*) List->Tail->Previous;
    newLast->DomainName = DomainName;
    newLast->IP = IP;
    newLast->TimeStamp = time(NULL);
    OldLast->Next = newLast;
    newLast->Previous = OldLast;
    newLast->Next = List->Tail;
    List->Tail->Previous = newLast; 
    printf("add list node after node:%s\n", OldLast->DomainName);
    printf("now list node is:%s\n", List->Tail->Previous->DomainName);
    printf("tail previous ptr:%p\n", List->Tail->Previous);
    printf("add tail ptr:%p\n", List->Tail);
    printf("now list node is:%s\n", OldLast->Next->DomainName);
    printf("oldLast ptr:%p\n", OldLast);
}
void HashAddFirst(HashLinkedList * List, char * IP, char* DomainName) {
    HashNode* OldFirst = List->Head->Next;
    HashNode* newFirst =(HashNode*) malloc(sizeof(HashNode));
    newFirst->DomainName = DomainName;
    newFirst->IP = IP;
    newFirst->TimeStamp = time(NULL);
    newFirst->Next = OldFirst;
    newFirst->Previous = List->Head;
    OldFirst->Previous = newFirst;
    List->Head->Next = newFirst;
}
void HashprintList(HashLinkedList* List) {
    HashNode* p = List->Head->Next;
    while (p != List->Tail){
        printf("IP: ");
        printf(p->IP);
        printf("  DomainName:");
        printf(p->DomainName);
        printf("  TImeStamp:");
        printf("%lld",p->TimeStamp);
        printf("\n");
        p = p->Next;
    }
}