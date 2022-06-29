- 要使用链表时请 `#include"LinkedList.h"`
- 大约是模仿了面向对象的思想写的

```c
//得到一个新的LinkedList对象：
LinkedList* l = GetEmptyList();//返回的是一个指向链表的指针，实质和Java的并无不同
//以下是实现的属于链表范畴的功能：
/*对链表尾插，第一个参数是要被插入的链表，后俩是IP和域名*/
void AddLast(LinkedList * List, char * IP, char* DomainName);
/*头插*/
void AddFirst(LinkedList* List, char * IP, char* DomainName);
/*移除链表头部的节点，并返回指向这个节点的指针*/
Node* RemoveFirst(LinkedList* List);
/*移除链表尾部的节点，并返回指向这个节点的指针*/
Node* RemoveLast(LinkedList* List);
/*链表为空返回1，否则返回0*/
int isEmpty(LinkedList* List);
/*链表的Size*/
int Size(LinkedList* List);
/*打印链表，测试用的*/
void printList(LinkedList* List);
/*得到第i个节点，返回这个节点的指针*/
Node* get(LinkedList* List, int index);
```

