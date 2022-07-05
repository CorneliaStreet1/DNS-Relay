#pragma once
#include "LinkedListNode.h"
typedef struct Linked_List
{
    Node* Head;
    Node* Tail;
    int Size; 
} LinkedList;

LinkedList * GetEmptyList();
void AddLast(LinkedList * List, char * IP, char* DomainName);
void AddFirst(LinkedList* List, char * IP, char* DomainName);
Node* RemoveFirst(LinkedList* List);
Node* RemoveLast(LinkedList* List);
int isEmpty(LinkedList* List);
int Size(LinkedList* List);
void printList(LinkedList* List);
Node* get(LinkedList* List, int index);
void RemoveNode(LinkedList* List, char* DomainName);


