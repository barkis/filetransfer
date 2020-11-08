#pragma once
#define __USE_W32_SOCKETS
#include <string>
#include <vector>
#include "tcpsock.h"
//#include<boost/thread.hpp>
#include <cstdint>
#include <map>
#include <thread>

class Linbak	{
private:
	std::string m_sConfigPath;
	int m_ConfigPort;
	short m_CountWidth;
	std::string m_sPathRoot;
	std::string m_sPathExtra;
	std::vector<std::string> m_excludedDirectories;
	tcpsock *m_pSocket;

//	void sendmess(std::string mess);
	//void recthread(/*void* pData*/);
	//std::thread *m_pReceiveThread;
	//std::thread *m_pHandleThread;
	std::thread *m_pReceiveThread;
	std::thread *m_pHandleThread;
	std::map<uint16_t, std::string> m_mapFileSequence,m_mapDirSequence;
	std::uint16_t m_FileSequence,m_DirSequence;

	void FindFileNamesAndDates(const std::string sPath,std::vector<WIN32_FIND_DATA> &vFileDetails, 
		std::vector<std::string> &vDirectories);
	void SendFileNamesAndDates(const std::vector<WIN32_FIND_DATA> &vFileDetails);
	void HandleRequests();
	void SendFile(int iSeq, std::string sName);
	void ReadConfig(std::string sFileName);
public:
	Linbak(std::string sPathExtra = "");
	~Linbak();
	void dirlist(std::string directory= "",int depth=0);
};
