#include<stdio.h>
#include<stdlib.h>

#include<WinSock2.h>
#include<winsock.h>
#include<Windows.h>
#include<WS2tcpip.h>
#include<mstcpip.h>
#include<time.h>
#include<malloc.h>

#include"HashCacheTable.h"
#include"serachTable.h"

#pragma comment(lib,"Ws2_32.lib")

#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)

#define IDNUM 128
#define BUFNUM 1024
#define	IDTIME 2

//typedef struct sockaddr_in sockaddr_in;
//typedef struct sockaddr sockaddr;

//ip地址和中继的sock
sockaddr_in myaddr, recvaddr, dnsaddr;
SOCKET mySock;

char buf[BUFNUM] = { 0 };

fnode* fileTable=NULL;

HashCacheTable* cache=NULL;

//初始化sock
void initSock() {
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);



	

	mySock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//return -1处理
	BOOL bNewBehavior = FALSE;
	DWORD dwBytesReturned = 0;
	WSAIoctl(mySock, SIO_UDP_CONNRESET, &bNewBehavior, sizeof bNewBehavior, NULL, 0, &dwBytesReturned, NULL, NULL);

	//地址
	myaddr, recvaddr, dnsaddr;

	myaddr.sin_family = AF_INET;
	myaddr.sin_port = htons(53);
	myaddr.sin_addr.S_un.S_addr = INADDR_ANY;

	char ipDefaultDNS[20] = "192.168.1.1";//10.3.9.44/192.168.1.1
	char ipOtherDNS[20] = { 0 };
	char ipDNS[20] = { 0 };

	memcpy(ipDNS, ipDefaultDNS, 20);

	dnsaddr.sin_family = AF_INET;
	dnsaddr.sin_port = htons(53);
	inet_pton(AF_INET, ipDNS, &dnsaddr.sin_addr);

	bind(mySock, (sockaddr*)&myaddr, sizeof(struct sockaddr));

	fileTable = init();

	cache = GetNewCacheTable();

	printf("初始化完成\n");
}

//DNS报文结构
struct header {
	unsigned short id;
	struct {
		unsigned char qr : 1;
		unsigned char opcode : 4;
		unsigned char aa : 1;
		unsigned char tc : 1;
		unsigned char rd : 1;
		unsigned char ra : 1;
		unsigned char z : 3;
		unsigned char rcode : 4;
	}flags;
	unsigned short qdcount;
	unsigned short ancount;
	unsigned short nscount;
	unsigned short arcount;
};

struct question {
	char* qname;
	unsigned short nameLen;
	char* realname;
	unsigned short realLen;//包括结尾的\0
	unsigned short qtype;
	unsigned short qclass;
	struct question* next;
};

struct rr {
	char* name;
	unsigned short nameLen;
	char* realname;
	unsigned short realLen;
	unsigned short type;
	unsigned short rclass;
	unsigned int ttl;
	unsigned short rdLen;
	char* rdData;
	struct rr* next;
};

struct DNSmessage {
	struct header h;
	struct question* q;
	struct rr* answer;
	struct rr* authority;
	struct rr* additional;
};

//string和message的相互转化

int getRealLen(char* buf, char* bufBegin) {
	int len = 0;
	for (; buf[len] != 0 &&(unsigned char) buf[len] < 0xC0; ++len);//一定要记得加unsigned char
	if (buf[len] == 0) {
		return len+1;
	}
	else {
		unsigned short offset = 0;
		memcpy(&offset, &buf[len], 2);
		offset = ntohs(offset);
		offset -= 0xC000;
		return getRealLen(bufBegin + offset, bufBegin)+len;
	}
}

void getRealName(char* buf, char* bufBegin, char** pRealname,int pNum) {
	int len = 0;
	for (; buf[len] != 0 &&(unsigned char) buf[len] < 0xC0; ++len,++pNum) {
		(*pRealname)[pNum] = buf[len];
		printf("%02X ",(unsigned char) buf[len]);
	}
	printf("\n");
	if (buf[len] == 0) {
		(*pRealname)[pNum] = 0;
		printf("get realname:%s\n", *pRealname);
		return;
	}
	else {
		unsigned short offset = 0;
		memcpy(&offset, &buf[len], 2);
		offset = ntohs(offset);
		offset -= 0xC000;
		getRealName(bufBegin + offset, bufBegin, pRealname, pNum);
	}
}

int getDomainName(char* buf,char* bufBegin,char** pname,char** pRealname,unsigned short* pnameLen,unsigned short* pRealLen) {
	int i = 0;//nameLen
	//printf("namelen buf: ");
	for (; buf[i] != 0 && (unsigned char)buf[i] < 0xC0; ++i) {
		//printf("%d:%02X ",i, (unsigned char)buf[i]);
	}
	//printf("\n");
	if (buf[i] == 0) {
		i++;
		*pname = (char*)malloc(sizeof(char) * i);
		*pnameLen = i;
		memcpy(*pname, buf, i);
	}
	else {
		i += 2;
		*pname = (char*)malloc(sizeof(char) * i);
		*pnameLen = i;
		memcpy(*pname, buf, i);
	}
	printf("name:%s\n", *pname);
	for (int k = 0; k < i; ++k) {
		printf("%02X ", (* pname)[k] );
	}
	printf("\n");
	*pRealLen = getRealLen(*pname, bufBegin);
	printf("realLen:%d\n", *pRealLen);
	*pRealname = (char*)malloc(sizeof(char) * (*pRealLen));
	getRealName(*pname, bufBegin, pRealname, 0);
	return i;
}

unsigned short read16bit(char** pbuf) {
	unsigned short value;
	memcpy(&value, *pbuf, 2);
	//printf("value:%d\n", value);
	*pbuf += 2;
	return ntohs(value);
}

unsigned int read32bit(char** pbuf) {
	unsigned int value;
	memcpy(&value, *pbuf, 4);
	*pbuf += 4;
	return ntohl(value);
}

void write16bit(char** pbuf, unsigned short value) {
	value = htons(value);
	memcpy(*pbuf, &value, 2);
	*pbuf += 2;
}

void write32bit(char** pbuf, unsigned int value) {
	value = htonl(value);
	memcpy(*pbuf, &value, 4);
	*pbuf += 4;
}

//string转message
void string2header(char** pbuf, struct header* h) {
	h->id = read16bit(pbuf);
	
	unsigned short flag = read16bit(pbuf);

	//printf("flag:%d\n", flag);

	h->flags.qr = (flag & (0x8000)) >> 15;
	//printf("flag.qr:%d\n", h->flags.qr);
	h->flags.opcode = (flag & (0x7800)) >> 11;
	//printf("flag.opcode:%d\n", h->flags.opcode);
	h->flags.aa = (flag & (0x400)) >> 10;
	h->flags.tc = (flag & (0x200)) >> 9;
	h->flags.rd = (flag & (0x100)) >> 8;
	h->flags.ra = (flag & (0x80)) >> 7;
	h->flags.z = (flag & (0x70)) >> 4;
	h->flags.rcode = (flag & (0xF));

	h->qdcount = read16bit(pbuf);
	h->ancount = read16bit(pbuf);
	h->nscount = read16bit(pbuf);
	h->arcount = read16bit(pbuf);
}

void string2question(char** pbuf, char* bufBegin, struct question* midQ, int count) {
	for (int i = 0; i < count; ++i) {
		int nameLen=getDomainName(*pbuf,bufBegin,&midQ->qname,&midQ->realname,&midQ->nameLen,&midQ->realLen);
		*pbuf += nameLen;
		
		midQ->qtype = read16bit(pbuf);
		midQ->qclass = read16bit(pbuf);

		if (i + 1 < count) {
			midQ->next = (struct question*)malloc(sizeof(struct question));
			midQ = midQ->next;
		}
		else {
			midQ->next = NULL;
		}
	}
}

void string2rr(char** pbuf, char* bufBegin, struct rr* r, int count) {
	for (int i = 0; i < count; ++i) {
		int nameLen = getDomainName(*pbuf,bufBegin,&r->name,&r->realname,&r->nameLen,&r->realLen);
		*pbuf += nameLen;
		//printf("nameLen:%d\n", nameLen);

		r->type = read16bit(pbuf);
		//printf("type:%X\n", r->type);
		//printf("type:%d\n", r->type);
		r->rclass = read16bit(pbuf);
		r->ttl = read32bit(pbuf);//4个字节
		//printf("ttl:%X\n", r->ttl);
		r->rdLen = read16bit(pbuf);

		r->rdData = (char*)malloc(sizeof(char) * (r->rdLen));
		printf("rdLen:%X\n", r->rdLen);
		memcpy(r->rdData, *pbuf, r->rdLen);
		*pbuf += r->rdLen;

		if (i + 1 < count) {
			r->next = (struct rr*)malloc(sizeof(struct rr));
			r = r->next;
		}
		else {
			r->next = NULL;
		}
	}
}

void string2message(char* buf,struct DNSmessage* message) {
	char* bufBegin = buf;
	//复制header
	string2header(&buf, &message->h);

	//复制question，用链表形式
	//printf("q\n");
	if (message->h.qdcount > 0) {
		message->q = (struct question*)malloc(sizeof(struct question));
	}
	string2question(&buf,bufBegin, message->q, message->h.qdcount);
	

	//复制answer
	//printf("ans\n");
	if (message->h.ancount > 0) {
		message->answer = (struct rr*)malloc(sizeof(struct rr));
	}
	string2rr(&buf,bufBegin, message->answer, message->h.ancount);

	//复制authority
	//printf("aut\n");
	if (message->h.nscount > 0) {
		message->authority = (struct rr*)malloc(sizeof(struct rr));
	}
	string2rr(&buf, bufBegin, message->authority, message->h.nscount);

	//复制addtional
	//printf("addtional\n");
	if (message->h.arcount > 0) {
		message->additional = (struct rr*)malloc(sizeof(struct rr));
	}
	string2rr(&buf, bufBegin, message->additional, message->h.arcount);
}

//message转string
void header2string(struct header* h, char** pbuf) {
	write16bit(pbuf, h->id);
	
	unsigned short flag=0,mid;
	/*mid = h->flags.qr;//之后改改
	//printf("write qr:%d\n", h->flags.qr);
	flag |= mid << 15;
	mid = h->flags.opcode;
	flag |= mid << 11;*/

	flag |= (h->flags.qr) << 15;
	flag |= (h->flags.opcode) << 11;
	
	flag |= (h->flags.aa) << 10;
	flag |= (h->flags.tc) << 9;
	flag |= (h->flags.rd) << 8;
	flag |= (h->flags.ra) << 7;
	flag |= (h->flags.z) << 4;
	flag |= (h->flags.rcode);
	//printf("flag:%X", flag);

	write16bit(pbuf, flag);
	

	write16bit(pbuf, h->qdcount);
	write16bit(pbuf, h->ancount);
	write16bit(pbuf, h->nscount);
	write16bit(pbuf, h->arcount);
}

void question2string(struct question* midQ, char** pbuf, int count) {
	for (int i = 0; i < count; ++i) {
		memcpy(*pbuf, midQ->qname, midQ->nameLen);
		*pbuf += midQ->nameLen;

		write16bit(pbuf, midQ->qtype);
		write16bit(pbuf, midQ->qclass);

		midQ = midQ->next;
	}
}

void rr2string(struct rr* r, char** pbuf, int count) {
	for (int i = 0; i < count; ++i) {
		memcpy(*pbuf, r->name, r->nameLen);
		*pbuf += r->nameLen;
		//printf("write nameLen:%d\n", r->nameLen);


		write16bit(pbuf, r->type);
		//printf("write type:%X\n", r->type);
		write16bit(pbuf, r->rclass);
		write32bit(pbuf, r->ttl);//4个字节
		write16bit(pbuf, r->rdLen);
		//printf("write rdLen:%X\n", r->rdLen);

		memcpy(*pbuf, r->rdData, r->rdLen);
		*pbuf += r->rdLen;
		r = r->next;
	}
}

int message2string(struct DNSmessage* message, char* buf) {
	char* originBuf = buf;
	//复制header
	header2string(&message->h, &buf);

	//复制question
	question2string(message->q, &buf, message->h.qdcount);

	//复制answer
	rr2string(message->answer, &buf, message->h.ancount);

	//复制authority
	rr2string(message->authority, &buf, message->h.nscount);

	//复制additional
	rr2string(message->additional, &buf, message->h.arcount);

	int bufLen = buf - originBuf;
	return bufLen;
}

void addRR(	struct DNSmessage* message,char* name, unsigned short nameLen, char* realname, 
			unsigned short realLen, unsigned short type, unsigned short rclass, unsigned int ttl, 
			unsigned short rdLen, char* rdData) 
	{
	struct rr* r = (struct rr*)malloc(sizeof(struct rr));
	r->name = name;
	r->nameLen = nameLen;
	printf("name:%s\nnameLen:%d\n", name, nameLen);

	r->realname = realname;
	r->realLen = realLen;
	r->type = type;
	r->rclass = rclass;
	r->ttl = ttl;
	r->rdLen = rdLen;
	r->rdData = rdData;
	struct rr* next = NULL;
	if (message->answer != NULL) {
		next = message->answer->next;
	}
	message->answer = r;
	message->h.ancount++;
	r->next = next;
}


//删除DNSmessage
void delQuestion(struct question* q) {
	struct question* next;
	for (; q != NULL; q = next) {
		next = q->next;
		free(q->qname);
		free(q->realname);
		free(q);
	}
}

void delRR(struct rr* r) {
	struct rr* next;
	for (; r != NULL; r = next) {
		next = r->next;
		free(r->name);
		printf("name free\n");
		free(r->realname);
		printf("real name free\n");
		free(r->rdData);
		printf("rdData free\n");
		free(r);
		printf("r free\n");
	}
}

void delMessage(struct DNSmessage* message) {
	delQuestion(message->q);
	delRR(message->answer);
	delRR(message->authority);
	delRR(message->additional);
	printf("delMessage finish\n");
}

//打印DNSmessage
void printMessage(struct DNSmessage message) {
	printf("DNSmessage info\n");
	printf("id:%d\nqr:%d\n", message.h.id, message.h.flags.qr);
}


//正常域名和DNS域名的相互转化
void domain2DNSdomain(char* domain, char* DNSdomain) {
	printf("domain2DNSdomain begin\n");
	printf("domain:%s", domain);
	int i = 0, j = 0;
	int partLen = 0;
	for (; domain[i] != '\0'; ++i) {
		if (domain[i] == '.') {
			DNSdomain[j] = partLen;
			++j;
			for (int k = i - partLen; domain[k] != '.'; k++,j++) {
				DNSdomain[j] = domain[k];
			}
			partLen = 0;
		}
		else {
			++partLen;
		}
	}
	DNSdomain[j] = partLen;
	j++;
	for (int k = i - partLen; domain[k] != '\0'; k++, j++) {
		DNSdomain[j] = domain[k];
	}
	DNSdomain[j] = 0;
	printf("DNSdomain:%s\n", DNSdomain);
	printf("domain2DNSdomain end\n");
}

void DNSdomain2domain(char* DNSdomain, char* domain) {
	printf("DNSdomain2domain begin\n");
	printf("DNSdomain:%s\n", DNSdomain);
	int i = 0, j = 0, partLen = 0;
	for (; DNSdomain[j] != 0;) {
		partLen = DNSdomain[j];
		++j;
		for (int k = 0; k < partLen; k++,j++,i++) {
			domain[i] = DNSdomain[j];
		}
		partLen = 0;
		domain[i] = '.';
		i++;
	}
	domain[i - 1] = '\0';
	printf("test\n");
	printf("DNSdomain:%s\n", DNSdomain);
	for (int m = 0; DNSdomain[m] != '\0'; ++m) {
		printf("%02X ", (unsigned char)DNSdomain[m]);
	}
	printf("\n");
	printf("domain:%s\n", domain);
	printf("DNSdomain2domain end\n");
}


//id映射
struct newId {
	short oldId;
	sockaddr_in cilentaddr;
	time_t endTime=0;
};

struct newId idTable[IDNUM];

void changeNewId(struct DNSmessage* message,sockaddr_in cilentaddr) {
	int oldId = message->h.id;
	time_t nowtime = time(NULL);
	int i;
	for (i = 0; i < IDNUM; ++i) {
		if (nowtime > idTable[i].endTime) {
			break;
		}
	}
	if (i == IDNUM) {
		printf("id-map error:id num too small\n");
		return;
	}
	idTable[i].oldId = message->h.id;
	idTable[i].cilentaddr = cilentaddr;
	idTable[i].endTime = nowtime + IDTIME;

	message->h.id = i;
	printf("new id:%d\n", message->h.id);
}

void changeOldId(struct DNSmessage* message,sockaddr_in* cilentaddr) {
	int id = message->h.id;
	time_t nowtime = time(NULL);
	if (nowtime > idTable[id].endTime) {
		printf("id-map error:id time too small\n");
	}
	else {
		printf("oldId:%d\n", idTable[id].oldId);
		message->h.id = idTable[id].oldId;
		*cilentaddr = idTable[id].cilentaddr;
		idTable[id].endTime = nowtime;
	}
}

//ip的字符串形式和int形式的相互转化
char* ipInt2ipChar(int ipInt) {
	unsigned char byteArray[4] = { 0 };
	unsigned int mask = 0xFF000000;
	int maskNum = 24;//是24，记得改
	for (int i = 0; i < 4; ++i,mask>>=4,maskNum-=8) {
		byteArray[i] = (ipInt & mask) >> maskNum;
	}


	char charArray[4][4] ;
	int len = 0;
	for (int i = 0; i < 4; ++i) {
		_itoa_s(byteArray[i], charArray[i],4, 10);
		len += strlen(charArray[i]);
	}

	char* ipChar=(char*)malloc(sizeof(char)*len);
	int ptrNum = 0;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; charArray[i][j] != '\0'; ++j) {
			ipChar[ptrNum] = charArray[i][j];
			ptrNum++;
		}
		if (i < 3) {
			ipChar[ptrNum] = '.';
			ptrNum++;
		}
		else {
			ipChar[ptrNum] = '\0';
		}
	}
	printf("ipChar:%s\n", ipChar);
	return ipChar;
}

int ipChar2ipInt(char* ipChar) {
	printf("ipChar2ipInt begin\n");
	char* charArray[4] = { 0 };
	char ip[20] = { 0 };
	strcpy_s(ip,20, ipChar);
	printf("copyip:%s\n", ip);
	charArray[0] = ip;
	int arrayNum = 1;
	for (int i = 0; ip[i] != '\0'; ++i) {
		if (ip[i] == '.') {
			ip[i] = '\0';
			charArray[arrayNum] = &ip[i + 1];
			arrayNum++;
		}
	}
	for (int i = 0; i < 4; ++i) {
		printf("ip part %d:%s\n", i, charArray[i]);
	}
	
	unsigned int byteMid;
	int ipInt = 0;
	int moveNum = 24;
	for (int i = 0; i < 4; ++i) {
		byteMid = atoi(charArray[i]);
		printf("byteMid %d:%d\n", i, byteMid);
		byteMid <<= moveNum;
		moveNum -= 8;
		ipInt += byteMid;
	}
	printf("ipInt:%X\n", ipInt);
	printf("ipChar2ipInt end\n");
	return ipInt;
}

int searchLocal(struct DNSmessage* message) {
	struct question* midQ = message->q;
	for (; midQ != NULL; midQ = midQ->next) {
		char* DNSdomainName = midQ->realname;
		char* realname = (char*)malloc(sizeof(char) * midQ->realLen);
		DNSdomain2domain(DNSdomainName, realname);
		Result* result = Get_Cached_IP_By_DomainName(cache, realname);
		if (result==NULL) {//cache没找到
			printf("cache没找到\n");
			printf("domain name:%s\n", realname);
			char* fileIP = serach(realname, fileTable);
			if (fileIP == NULL) {//txt没找到
				printf("文件没找到\n");
				delRR(message->answer);
				message->h.ancount = 0;
				return -1;
			}
			else {//txt找到了
				printf("文件找到了：\n");
				printf("ip:%s\n", fileIP);
				printf("domain:%s\n", realname);
				char* copyIP = (char*)malloc(sizeof(char) * 20);
				char* copyDomain = (char*)malloc(sizeof(char) * midQ->realLen);
				strcpy_s(copyIP, 20, fileIP);
				strcpy_s(copyDomain, midQ->realLen ,realname);
				AddNewItem(cache, copyDomain, copyIP);//加入cache

				if (strcmp(fileIP, "0.0.0.0") == 0) {
					printf("file查到屏蔽ip\n");
					delRR(message->answer);
					message->h.ancount = 0;
					message->h.flags.rcode = 3;
					return 0;
				}

				char* answerDNSdomain = (char*)malloc(sizeof(char) * midQ->realLen);
				domain2DNSdomain(realname, answerDNSdomain);

				int IPint = ipChar2ipInt(copyIP);
				IPint = htonl(IPint);
				char* rdData = (char*)malloc(sizeof(int));
				memcpy(rdData, &IPint, 4);
				printf("ipv4:\n");
				for (int i = 0; i < 4; ++i) {
					printf("%02X ", rdData[i]);
				}
				printf("\n");
				addRR(message, answerDNSdomain, midQ->realLen, NULL, 0, 1, 1, 86400, 4, rdData);
			}
		}
		else if(strcmp(result->IP,"0.0.0.0")==0) {//屏蔽ip
			printf("cache查找到屏蔽ip\n");
			delRR(message->answer);
			message->h.ancount = 0;
			message->h.flags.rcode = 3;
			message->h.flags.qr = 1;
			return 0;
		}
		else {//cache找到了
			printf("在cache里找到\n");
			char* answerDNSdomain = (char*)malloc(sizeof(char) * midQ->realLen);
			domain2DNSdomain(realname, answerDNSdomain);

			int IPint = ipChar2ipInt(result->IP);
			IPint = htonl(IPint);
			char* rdData = (char*)malloc(sizeof(int));
			memcpy(rdData, &IPint, 4);
			printf("ipv4:\n");
			for (int i = 0; i < 4; ++i) {
				printf("%02X ", rdData[i]);
			}
			printf("\n");
			addRR(message, answerDNSdomain, midQ->realLen, NULL, 0, 1, 1, result->TimeToLive, 4, rdData);
		}
	}
	return 1;
}

void dns2cache(struct DNSmessage* message) {
	printf("dns2cache begin\n");
	struct rr* r = message->answer;
	printf("answer:%X\n", message->answer);
	for (; r != NULL; r = r->next) {
		if (r->type == 1 && r->rclass==1 ) {
			char* domainName = (char*)malloc(sizeof(char) * r->realLen);
			DNSdomain2domain(r->realname, domainName);
			int ipInt = 0;
			memcpy(&ipInt, r->rdData, 4);
			ipInt = ntohl(ipInt);
			char* ipChar = ipInt2ipChar(ipInt);
			AddNewItem(cache, domainName, ipChar);
		}
	}
	printf("dns2cache end\n");
}


//发送接收模块
void recvAndSend() {
	int sLen = sizeof(struct sockaddr);
	int recvNum;
	recvNum=recvfrom(mySock, buf, BUFNUM, 0, (sockaddr*)&recvaddr, &sLen);

	if (recvNum < 0)
		wprintf(L"recvfrom failed with error %d\n", WSAGetLastError());

	char ip[20];
	inet_ntop(AF_INET, &recvaddr.sin_addr, ip, 20);
	printf("recv ip:%s\nrecv port:%d\nrecvNum:%d\nrecv buf:\n", ip,ntohs(recvaddr.sin_port), recvNum);
	for (int i = 0; i < recvNum; ++i) {
		if (i != 0 && i % 16 == 0) {
			printf("\n");
		}
		printf("%02X ", (unsigned char)buf[i]);
		
	}
	printf("\n");

	for (int i = 0; i < recvNum; ++i) {
		if (i != 0 && i % 16 == 0) {
			printf("\n");
		}
		printf("%c ", buf[i]);

	}
	printf("\n");

	
	if (recvNum > 0) {
		struct DNSmessage message;
		message.q = NULL;
		message.answer = NULL;
		message.authority = NULL;
		message.additional = NULL;

		string2message(buf, &message);
		printMessage(message);

		
		if (message.h.flags.qr == 0) {//接收到客户端的报文
			int isFind = searchLocal(&message);

			if (isFind == -1) {
				changeNewId(&message, recvaddr);
				inet_ntop(AF_INET, &recvaddr.sin_addr, ip, 20);
				printf("recvaddr ip:%s,port:%d\n", ip, ntohs(recvaddr.sin_port));
				int sendNum = message2string(&message, buf);

				printf("send buf:\n");
				for (int i = 0; i < sendNum; ++i) {
					if (i != 0 && i % 16 == 0) {
						printf("\n");
					}
					printf("%02X ", (unsigned char)buf[i]);
				}
				printf("\n");



				sendto(mySock, buf, sendNum, 0, (sockaddr*)&dnsaddr, sLen);
				inet_ntop(AF_INET, &dnsaddr.sin_addr, ip, 20);
				printf("send to dns\nip:%s\nport:%d\n", ip, ntohs(dnsaddr.sin_port));
			}
			else {
				message.h.flags.qr = 1;
				int sendNum = message2string(&message, buf);

				printf("send buf:\n");
				for (int i = 0; i < sendNum; ++i) {
					if (i != 0 && i % 16 == 0) {
						printf("\n");
					}
					printf("%02X ", (unsigned char)buf[i]);
				}
				printf("\n");



				sendto(mySock, buf, sendNum, 0, (sockaddr*)&recvaddr, sLen);
				inet_ntop(AF_INET, &recvaddr.sin_addr, ip, 20);
				printf("send to cilent\nip:%s\nport:%d\n", ip, ntohs(recvaddr.sin_port));
			}
		}
		else {//接收到服务器的报文
			sockaddr_in sendaddr;
			changeOldId(&message, &sendaddr);
			printMessage(message);
			int sendNum = message2string(&message, buf);

			printf("send buf:\n");
			for (int i = 0; i < sendNum; ++i) {
				if (i != 0 && i % 16 == 0) {
					printf("\n");
				}
				printf("%02X ", (unsigned char)buf[i]);
			}
			printf("\n");

			printf("before send to dns\n");
			dns2cache(&message);
			sendto(mySock, buf, sendNum, 0, (sockaddr*)&sendaddr, sLen);
			inet_ntop(AF_INET, &sendaddr.sin_addr, ip, 20);
			printf("send to cilent\nip:%s\nport:%d\n", ip, ntohs(sendaddr.sin_port));
		}
		delMessage(&message);
	}
	printf("end\n\n\n");
}


int main() {
	initSock();
	while (1) {
		recvAndSend();
	}
	closesocket(mySock);
	WSACleanup();
	return 0;
}

