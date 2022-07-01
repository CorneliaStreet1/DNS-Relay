#include"HashTable.h"
#include"LinkedList.h"
typedef struct Hash_Cache_Table
{
    LinkedList* LRUList;
    struct Hash_Table* CacheTable;
} HashCacheTable;
HashCacheTable* GetNewCacheTable();
Result* Get_Cached_IP_By_DomainName(HashCacheTable* Table, char* DomainName);
void AddNewItem(HashCacheTable* Table, char* DomainName, char* IP);