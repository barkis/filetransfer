#include "sync.h"
#include <cstdlib>
#include<cstring>
#include <ctime>
#include "utcpsock.h"

Sync::Sync():m_FileNotOpened(false)	{
	ReadConfig("config.xml");
	char szLogFileName[101];
	strcpy(szLogFileName,getenv("HOME"));
	strcat(szLogFileName,"/sync.log");
	m_LogFile.open(szLogFileName,std::ios::out);
	if(!m_LogFile)	{
		std::cerr << "Failed to open log file" << '\n';
	}

	char szStart[71];
	time_t clock;
	time(&clock);
	
	strcpy(szStart,"Starting sync at ");
	strcat(szStart,ctime(&clock));
	Log(szStart);

	m_pSocket = new tcpsock(m_ConfigPort);
	m_pBuff = new Buffer(m_pSocket);
}


void Sync::MakeSockConnection()	{
	m_pSocket->GetConnection();
}
 void Sync::CloseSocks()	{
	m_pSocket->CloseReadSocket();
 }

Sync::~Sync()	{
	char szEnd[71];
	time_t clock;
	time(&clock);
	
	strcpy(szEnd,"Ending sync at ");
	strcat(szEnd,ctime(&clock));
	Log(szEnd);

	m_LogFile.close();
	delete m_pSocket;
	delete m_pBuff;
}
