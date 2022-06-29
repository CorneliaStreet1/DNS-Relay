#include<stdio.h>
#include"LinkedList.h"
int main(int argc, char const *argv[])
{
    LinkedList* l = GetEmptyList();
    AddLast(l, "114514", "HomoOfficial");
    AddLast(l, "1919180", "Taffy");
    AddLast(l, "127.0.0.1", "localhost");
    AddLast(l,"111.111.111.0", "Test1");
    AddFirst(l, "1.9.8.0", "boboxiaoya");
    printf("List:\n");
    printList(l);
    printf("\nSize: %d\n\n", Size(l));
    printf("get the third element:\n");
    printf("Expected:127.0.0.1_localhost\n");
    printf("Get:");
    Node* c = get(l, 3);
    printf(c->IP);
    printf("  ");
    printf(c->DomainName);
    printf("\n");
    printf("Test RemoveFirst:\n");
    while (!isEmpty(l))
    {
        printf("Removed one from head end: ");
        Node* n = RemoveFirst(l);
        printf("DN = ");
        printf(n->DomainName,  n->IP);
        printf(" IP = ");
        printf(n->IP);
        printf(" Size = %d", Size(l));
        printf("\n");
    }
    printf("\n");
    printf("Add all again");
    AddLast(l, "114514", "HomoOfficial");
    AddLast(l, "1919180", "Taffy");
    AddLast(l, "127.0.0.1", "localhost");
    AddLast(l,"111.111.111.0", "Test1");
    AddFirst(l, "1.9.8.0", "boboxiaoya");
    printf("\n");
    printf("Test RemoveLast:");
    printf("\n");
    while (!isEmpty(l))
    {
        printf("Removed one from tail end: ");
        Node* n = RemoveLast(l);
        printf("DN = ");
        printf(n->DomainName,  n->IP);
        printf(" IP = ");
        printf(n->IP);
        printf(" Size = %d", Size(l));
        printf("\n");
    }
    return 0;
}
