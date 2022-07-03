#define HASH_SIZE 199
#define FILE_SIZE 208

typedef struct Fnode
{
	char* ip;
	char* name;
	struct Fnode* next;
} fnode;

void getHash(fnode* hashTable, char** name, char** ip);
void exchangeLower(char* string);
char* serach(char*, fnode* hashTable);
fnode* init();
int getTable(char** ip, char** name);
int hashFunction(char* str);