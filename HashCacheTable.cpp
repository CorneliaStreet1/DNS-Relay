#include"HashCacheTable.h"
#include<stdlib.h>
#include<stdio.h>
HashCacheTable* GetNewCacheTable() {
    HashCacheTable* table = (HashCacheTable*)malloc(sizeof(HashCacheTable));
    table->CacheTable = GetEmptyHashTable();
    table->LRUList = GetEmptyList();
    return table;
}
Result* Get_Cached_IP_By_DomainName(HashCacheTable* Table, char* DomainName) {
    printf("Searching By DomainName: ");
    printf(DomainName);
    printf("\n");
    return Get_IP_By_DomainName(Table->CacheTable, DomainName,Table->LRUList);
}
void AddNewItem(HashCacheTable* Table, char* DomainName, char* IP) {
    int index = getIndex(DomainName);
    if (Table->CacheTable->size == MaxCacheNumber && Table->LRUList->Size == MaxCacheNumber)//����ԶԶ����75
    {
        printf("Cache full,replacing by LRU\n");
        AddNewRecord(Table->CacheTable, IP, DomainName);
        AddFirst(Table->LRUList, IP, DomainName);
        Node* n = RemoveLast(Table->LRUList);
        Remove(Table->CacheTable, n->IP, n->DomainName);//���name��ip��������
        printf("Removed record: IP: ");
        printf(n->IP);
        printf("  DomainName:");
        printf(n->DomainName);
        printf("\n");
    } else {
        AddFirst(Table->LRUList, IP, DomainName);
        AddNewRecord(Table->CacheTable, IP, DomainName);
        printf("Cache Not FUll, Added new Record:\n");
        printf("IP: ");
        printf(IP);
        printf("    DomainName: ");
        printf(DomainName);
        printf("\n");
    }
    printf("\n��ǰ����LRU\n");
    printList(Table->LRUList);
    printf("�����������\n\n");
}