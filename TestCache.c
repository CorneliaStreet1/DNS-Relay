#include"HashCacheTable.h"
#include<stdio.h>
#include<stdlib.h>
int main(int argc, char const *argv[])
{
    HashCacheTable* Table = GetNewCacheTable();
    for (int i = 0; i < 100; i++)
    {
        AddNewItem(Table, "HomoOfficial","11.45.14");
    }
    for (int i = 0; i < 45; i++)
    {
        Result* r = Get_Cached_IP_By_DomainName(Table, "HomoOfficial");
        printf("IP: ");
        printf(r->IP);
        printf("TTL: ");
        printf("%lld",r->TimeToLive);
        printf("\n");
    }
    return 0;
}
