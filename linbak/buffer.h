#ifndef BUFFER_H
#define BUFFER_H
#include <iostream>
class tcpsock;
class Sync;

class Buffer	{
public:
	static const int MAXBUFF = 1000;
private:
	tcpsock *m_pSocket;
	char m_Data[MAXBUFF];
	unsigned int m_Current;
	int m_DataLength;
	int read(char *pos,int len,Sync *pSync=nullptr);
public:
	Buffer(tcpsock *pTheSock);
	int GetNextChars(char *pTo,int len,bool zTerminate=true,Sync *pSync=nullptr);
	const char * dump(){return m_Data;}
};

#endif
