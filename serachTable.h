#define HASH_SIZE 199
#define FILE_SIZE 208

typedef struct node
{
	char* ip;
	char* name;
	struct node* next;
} node;

void getHash(node* hashTable, char** name, char** ip);
void exchangeLower(char* string);
char* serach(char*, node* hashTable);
node* init();
int getTable(char** ip, char** name);
int hashFunction(char* str);