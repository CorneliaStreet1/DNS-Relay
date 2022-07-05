cmake_minimum_required(VERSION 3.22)
project(DNS_Relay)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(DNS_Relay
        HashCacheTable.c
        HashCacheTable.h
        HashLinkedList.c
        HashLinkedList.h
        HashTable.c
        HashTable.h
        LinkedList.c
        LinkedList.h
        LinkedListNode.h
        LinkedListTest.c
        main.cpp
        serachTable.c
        serachTable.h
        TestCache.c)
