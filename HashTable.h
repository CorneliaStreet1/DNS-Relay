#include "HashLinkedList.h"
#include<stdlib.h>
#include<string.h>
#include<time.h>
#define TTL 86400
#define MaxCacheNumber 75
#define BucketNumber 100
typedef struct Hash_Table
{
    int size;
    HashLinkedList* Bucket[BucketNumber];
} HashTable;
HashTable* GetEmptyHashTable() {
    HashTable* hashTable = (HashTable*) malloc(sizeof(HashTable));
    hashTable->size = 0;
    for (int i = 0; i < BucketNumber; i++)
    {
        hashTable->Bucket[i] = GetEmptyList();
    }
}
int StringHashCode(const char * str) {
        int hash = 0;

    for (int i = 0; i < strlen(str); i++) {
        hash = 31 * hash + str[i];
    }
    return hash;
}
int NodeStringHashCode(const char* DomainName) {
    return StringHashCode(DomainName);
}
int getIndex(const char* DomainName) {
    return floorMod(NodeStringHashCode(DomainName), BucketNumber);
}
int floorMod(int x, int y) {
        int mod = x % y;
        // if the signs are different and modulo not zero, adjust result
        if ((mod ^ y) < 0 && mod != 0) {
            mod += y;
        }
        return mod;
}
char* Get_IP_By_DomainName(HashTable* Table, const char* DomainName) {
    int index = getIndex(DomainName);
    HashLinkedList* L = Table->Bucket[index];
    HashNode* p = L->Head->Next;
    while (strcmp(p->DomainName, DomainName) != 0 && p != L->Tail)
    {
        p = p->Next;
    }
    if (p == L->Tail || p == NULL)
    {
        return NULL;
    }
    else if (strcmp(p->DomainName, DomainName) == 0)
    {
        long long PastTime = time(NULL) - p->TimeStamp;
        if (PastTime >= TTL)
        {
            HashNode* Pre = p->Previous;
            HashNode* Next = p->Next;
            Pre->Next = Next;
            Next->Previous = Pre;
            free(p);
            return NULL;
        }
        else {
            return p->IP;
        }
        
    }
    return NULL;
}
void AddNewRecord(HashTable* Table,char* IP,char* DomainName) {
    int index = getIndex(DomainName);
    AddLast(Table->Bucket[index], IP, DomainName);
}