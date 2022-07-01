# 关于HashCache

- 用的是哈希表+摆烂实现的LRU。

- 会用到的只有`HashCacheTable.c` 和`HashTableCache.h`

- 手动编译下面的代码：`gcc TestCache.c  HashCacheTable.c LinkedList.c HashLinkedList.c HashTable.c -o TestCache`

- ```c
  //
  // Created by jiangyiqing on 2022/7/1.
  //
  #include"HashCacheTable.h"
  #include<stdio.h>
  int main(int argc, char const *argv[])
  {
      HashCacheTable* Table = GetNewCacheTable();
      for (int i = 0; i < 100; i++)
      {   
  /**
  *加入一个记录，会自动使用LRU，同时在控制台打印消息，指明是LRU替换还是直接加入
  */
          AddNewItem(Table, "HomoOfficial","11.45.14");
      }
      for (int i = 0; i < 45; i++)
      {
  /**
  *根据域名查询IP地址。摆烂版
  */
          Result* r = Get_Cached_IP_By_DomainName(Table, "HomoOfficial");
          printf("IP: ");
          printf(r->IP);
          printf("TTL: ");
          printf("%lld",r->TimeToLive);
          printf("\n");
      }
  
      return 0;
  }
  
  ```

  