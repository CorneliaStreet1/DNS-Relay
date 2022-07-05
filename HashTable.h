#include "HashLinkedList.h"
#include"LinkedList.h"
#define TTL 10
#define MaxCacheNumber 75
#define BucketNumber 100
typedef struct CacheResult
{
    char* IP;
    long long TimeToLive;
} Result;

struct Hash_Table
{
    int size;
    HashLinkedList* Bucket[BucketNumber];
};
struct Hash_Table * GetEmptyHashTable();
int StringHashCode(char * str);
int NodeStringHashCode(char* DomainName);
int floorMod(int x, int y);
int getIndex(char* DomainName);
Result* Get_IP_By_DomainName(struct Hash_Table* Table, char* DomainName,LinkedList* LRU);
void AddNewRecord(struct Hash_Table* Table,char* IP,char* DomainName);
void Remove(struct Hash_Table* Table,char* IP,char* DomainName);