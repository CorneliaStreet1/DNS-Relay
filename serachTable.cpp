#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"serachTable.h"

//int main() {
//	node* h=init();
//	char str[100]="";
//	scanf_s("%s", str,100);
//	char* ip = serach(str, h);
//	printf("%s", ip);
//}

fnode* init() {
	char* ip[FILE_SIZE] = { NULL };
	char* name[FILE_SIZE] = { NULL };
	//fnode hashTable[HASH_SIZE];
	fnode* hashTable = (fnode*)malloc(sizeof(fnode) * HASH_SIZE);
	for (int i = 0; i < HASH_SIZE; i++) {
		hashTable[i].next = NULL;
		hashTable[i].ip = NULL;
		hashTable[i].name = NULL;
	}
	if (-1 == getTable(ip, name)) {
		return NULL;
	}
	getHash(hashTable, name, ip);
	return hashTable;
}

int getTable(char**ip,char**name) {
	FILE* fp = NULL;
	errno_t errFile;
	if ((errFile = fopen_s(&fp, ".\\dnsrelay.txt", "r")) != 0) {
		printf("error occurred when opening file ");
		return -1;
	}
	char buf[80] = { 0 };
	int line = 0, lenIP = 0, lenName = 0;
	
	while (NULL != fgets(buf, 80, fp)) {
		exchangeLower(buf);
		if (buf[strlen(buf) - 1] == '\n') {
			buf[strlen(buf) - 1] = '\0';
		}
		lenName = strlen(buf);
		for (lenIP = 0; buf[lenIP++] != ' '; ) {
		}
		if (line == 0||  strcmp(name[line - 1], &buf[lenIP]) != 0) {
			ip[line] = (char*)malloc(2 * lenIP);
			name[line] =(char*) malloc(2 * (lenName - lenIP+1));
			int i;
			for (i = 0; i < lenIP - 1; i++) {
				ip[line][i] = buf[i];
			}
			ip[line][i] = '\0';
			for (i = 0; i < lenName - lenIP ; i++) {
				name[line][i] = buf[lenIP + i];
			}
			name[line][i] = '\0';
			//printf("%s\n", ip[line]);
			//printf("%s\n", name[line]);
			line++;
		}
	}
	fclose(fp);
	fp = NULL;
	return 0;
}

int hashFunction(char*str) {
	int hash = 0;
	while (*str) {
		hash += (*str) * (*str)+(*str);
		str++;
	}
	hash = hash % HASH_SIZE;
	return hash >0?hash:(-1*hash);
}

void getHash(fnode* hashTable,char**name,char**ip) {
	for (int i = 0; name[i]!=NULL; i++) {
		int hash = hashFunction(name[i]);
		if (hashTable[hash].name == NULL) {
			hashTable[hash].name = name[i];
			hashTable[hash].ip = ip[i];
			hashTable[hash].next = NULL;
		}
		else {
			fnode* newNode = (fnode*)malloc(sizeof(fnode));
			(*newNode).ip = ip[i];
			(*newNode).name = name[i];
			(*newNode).next = NULL;
			fnode* temp = &hashTable[hash];
			while ((*temp).next != NULL) {
				temp = (*temp).next;
			}
			(*temp).next = newNode;
		}
	}
}

void exchangeLower(char*str) {
	while ((*str) != '\0') {
		if ((*str) >='A' && (*str) <='Z') {
			char temp = (*str) + 32;
			(*str) = temp;
		}
		str++;
	}
}

char* serach(char*str,fnode* hashTable) {//传给这个函数的str记得复制一遍再传，因为改了大小写
	//printf("search file begin\n");
	exchangeLower(str);
	int hash = hashFunction(str);
	//printf("hashFunction is ok\n");
	fnode* temp = &hashTable[hash];
	/*if (temp == NULL) {
		printf("temp is NULL\n");
	}
	else {
		printf("name:%s\n", temp->name);
	}*/
	if (hashTable[hash].name == NULL) {
		//printf("search short end\n");
		return NULL;
	}
	//printf("while begin\n");
	while (temp != NULL) {
		printf("1\n");
		if (temp->name == NULL) {
			printf("name == NULL, error\n");
		}
		//printf("temp.next:%X\n", temp->next);
		//printf("temp.name:%s\n", temp->name);
		if (strcmp((*temp).name, str) == 0) {
			//printf("temp ip:%s\n", temp->ip);
			return (*temp).ip;
		}
		
		temp = (*temp).next;
	}
	//printf("search end\n");
	return NULL;
}