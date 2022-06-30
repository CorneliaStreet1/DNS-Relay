#include<stdio.h>
#include<stdlib.h>

#include"serachTable.h"

//int main() {
//	node* h=init();
//	char str[100]="";
//	scanf_s("%s", str,100);
//	char* ip = serach(str, h);
//	printf("%s", ip);
//}

node* init() {
	char* ip[FILE_SIZE] = { NULL };
	char* name[FILE_SIZE] = { NULL };
	node hashTable[HASH_SIZE];
	for (int i = 0; i < HASH_SIZE; i++) {
		hashTable[i].next = hashTable[i].ip = hashTable[i].name = NULL;
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
			ip[line] = malloc(2 * lenIP);
			name[line] = malloc(2 * (lenName - lenIP+1));
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

void getHash(node* hashTable,char**name,char**ip) {
	for (int i = 0; name[i]!=NULL; i++) {
		int hash = hashFunction(name[i]);
		if (hashTable[hash].name == NULL) {
			hashTable[hash].name = name[i];
			hashTable[hash].ip = ip[i];
			hashTable[hash].next = NULL;
		}
		else {
			node* newNode = (node*)malloc(sizeof(node));
			(*newNode).ip = ip[i];
			(*newNode).name = name[i];
			(*newNode).next = NULL;
			node* temp = &hashTable[hash];
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

char* serach(char*str,node* hashTable) {
	exchangeLower(str);
	int hash = hashFunction(str);
	node* temp = &hashTable[hash];
	if (hashTable[hash].name == NULL) {
		return NULL;
	}
	while (temp != NULL) {
		if (strcmp((*temp).name, str) == 0) {
			return (*temp).ip;
		}
		temp = (*temp).next;
	}
	return NULL;
}