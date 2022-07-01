#include"hashtable.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
struct Hash_Table* GetEmptyHashTable() {
    struct Hash_Table* hashTable =  malloc(sizeof(struct Hash_Table));
    hashTable->size = 0;
    for (int i = 0; i < BucketNumber; i++)
    {
        hashTable->Bucket[i] = GetEmptyHashList();
    }
    return hashTable;
}
int StringHashCode(char * str) {
        int hash = 0;

    for (int i = 0; i < strlen(str); i++) {
        hash = 31 * hash + str[i];
    }
    return hash;
}
int NodeStringHashCode(char* DomainName) {
    return StringHashCode(DomainName);
}
int floorMod(int x, int y) {
        int mod = x % y;
        // if the signs are different and modulo not zero, adjust result
        if ((mod ^ y) < 0 && mod != 0) {
            mod += y;
        }
        return mod;
}
int getIndex(char* DomainName) {
    return floorMod(NodeStringHashCode(DomainName), BucketNumber);
}

Result* Get_IP_By_DomainName(struct Hash_Table* Table, char* DomainName) {
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
            Result* r = malloc(sizeof(Result));
            r->IP = p->IP;
            r->TimeToLive = time(NULL) - p->TimeStamp;
            return r;
        }
        
    }
    return NULL;
}
void AddNewRecord(struct Hash_Table* Table,char* IP,char* DomainName) {
    int index = getIndex(DomainName);
    Table->size ++;
    HashAddLast(Table->Bucket[index], IP, DomainName);
}
void Remove(struct Hash_Table* Table,char* IP,char* DomainName) {
    int index = getIndex(DomainName);
    HashLinkedList* L = Table->Bucket[index];
    HashNode* p = L->Head->Next;
    while (strcmp(p->DomainName, DomainName) != 0 && p != L->Tail)
    {
        p = p->Next;
    }
    if (p == NULL || p == L->Tail)
    {
        return;
    }
    
    if (strcmp(p->DomainName, DomainName) == 0)
    {
        if (strcmp(p->IP, IP) == 0) {
            HashNode* Pre = p->Previous;
            HashNode* Next = p->Next;
            Pre->Next = Next;
            Next->Previous = Pre;
            free(p);
            Table->size --;
        }
        else {
            printf("Remove Error,IP:  ");
            printf(IP);
            printf("  Domain:  ");
            printf(DomainName);
        }
    }
}