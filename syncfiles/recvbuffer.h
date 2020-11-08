#ifndef BUFFER_H
#define BUFFER_H
#include <iostream>
class tcpsock;

class Buffer	{
public:
	static const int MAXBUFF = 1000;
private:
	tcpsock *m_pSocket;
	char m_Data[MAXBUFF];
	unsigned int m_Current;
	int m_DataLength;
	int read(char *pos,int len);
public:
	Buffer(tcpsock *pTheSock);
	int GetNextChars(char *pTo,int len,bool zTeminat=true);
};

#endif
