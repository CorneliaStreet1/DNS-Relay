#include"hashtable.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
struct Hash_Table* GetEmptyHashTable() {
    struct Hash_Table* hashTable = (Hash_Table*) malloc(sizeof(struct Hash_Table));
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
    printf("getIpByDomain begin\n");
    int index = getIndex(DomainName);
    printf("index:%d\n", index);
    HashLinkedList* L = Table->Bucket[index];
    printf("bound is ok\n");
    HashNode* p = L->Head->Next;
    printf("get ip list tail:%p\n", L->Tail);
    printf("get ip list head:%p\n", L->Head);
    if (p == NULL) {
        printf("p is NULL\n");
    }
    else {
        printf("p.domainname:%s\n", p->DomainName);
    }
    /*while (p->DomainName != NULL && strcmp(p->DomainName, DomainName) != 0 && p != L->Tail)
    {
        printf("p.domainName:%s\n", p->DomainName);
        p = p->Next;
    }*/

    while ( p != L->Tail)
    {
        if (p->DomainName != NULL && strcmp(p->DomainName, DomainName) == 0) {
            printf("break\n");
            break;
        }
        printf("p.domainName:%s\n", p->DomainName);
        printf("p ptr:%p\n", p);
        printf("p.domainName.next:%s\n", p->Next->DomainName);
        p = p->Next;
    }

    printf("while end\n");
    if (p == L->Tail || p == NULL)
    {
        printf("空\n");
        return NULL;
    }
    else if (strcmp(p->DomainName, DomainName) == 0)
    {
        printf("非空\n");
        long long PastTime = time(NULL) - p->TimeStamp;
        if (PastTime >= TTL)
        {
            HashNode* Pre = p->Previous;
            HashNode* Next = p->Next;
            Pre->Next = Next;
            Next->Previous = Pre;
            printf("del pastTime p name:%s\n", p->DomainName);
            printf("del pastTime p ptr:%p\n", p);
            free(p);//这里去掉这个节点没减长度
            return NULL;
        }
        else {
            Result* r =(Result*) malloc(sizeof(Result));
            r->IP = p->IP;
            r->TimeToLive = time(NULL) - p->TimeStamp;
            return r;
        }
        
    }
    printf("getIpByDomain end\n");
    return NULL;
}
void AddNewRecord(struct Hash_Table* Table,char* IP,char* DomainName) {
    int index = getIndex(DomainName);
    Table->size ++;
    HashAddLast(Table->Bucket[index], IP, DomainName);
}
void Remove(struct Hash_Table* Table,char* IP,char* DomainName) {
    printf("remove begin\n");
    int index = getIndex(DomainName);
    HashLinkedList* L = Table->Bucket[index];
    HashNode* p = L->Head->Next;
    printf("p.domainName:%s\np.ip:%s\n", p->DomainName, p->IP);
    while (p != L->Tail && strcmp(p->DomainName, DomainName) != 0 )
    {
        p = p->Next;
    }
    if (p == NULL || p == L->Tail)
    {
        printf("remove end\n");
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
    printf("remove end\n");
}