#include <string>
#include <fstream>
#include <vector>
#include <dirent.h>
#include <map>
#include <cstdio>
#include <exception>
#include <iostream>
#include "buffer.h"

class ReadFailed: public std::exception
{
	std::string m_sText;
	int m_Code;
public:
	ReadFailed(std::string sMess,int errorCode):m_sText(sMess),m_Code(errorCode){}
	virtual const char* what();
}; 

class tcpsock;

class Sync	{
	static const short m_COUNTWIDTH = 4;
	static const int PORT = 23001;
	static const char *WINROOT;
	static const char *LINROOT;
	char m_szCount[m_COUNTWIDTH + 1];
	std::vector<std::string>m_vWinDates;
	std::vector<std::string>  m_vLinFiles; 
	std::vector<std::string> m_vLinDates;
	std::vector<std::string> GetDiff();
	std::string m_sClientMachine;
	std::string m_sPathRoot;
	tcpsock *m_pSocket;
	std::string m_sCurrentDirForUpdate;
	std::string m_sCurrentFileForUpdate;
	std::map<unsigned int,std::string> m_mapSeqDir;
	std::ofstream m_TempFileStream;
	char m_szTempFileName[101];
	char m_cType;
	unsigned int m_Sequence;
	int m_NextLen;
	char m_szNext[Buffer::MAXBUFF - m_COUNTWIDTH - 1];
	//char *m_szNext;
	Buffer *m_pBuff;
	static const int m_InputBlockSize = 1000;
	int m_RequestsOutstanding;
	bool m_EndReceived;
	std::ofstream m_LogFile;
	std::string m_sConfigPath; 
	int m_ConfigPort; 
	short m_CountWidth; 
	bool m_FileNotOpened;	//failed to open file, discard updates

	void Initialise(std::string sInitBlock);
	DIR* GetFiles(std::string sDirPath);	
	DIR * DirExistsOrMake(std::string DirName);
	DIR* PathMake(std::string DirName);
	void DoUpdates(char *szNext,unsigned int iDSeq);
	unsigned int Sequence(unsigned char* aiSeq);
	void AddToVectors(char *szFileDetails);
	void RequestUpdates(std::vector<std::string> &vFileNames, unsigned int iDSeq);	
	void StartFileUpdate(char *szFName, unsigned int iDSeq);
	void EndFileUpdate();
	void read();
	void ReadDir(char *szNext);
	void ReadConfig(std::string sFileName); 
public:
	Sync();
	~Sync();
	void MainLoop();
	void MakeSockConnection();
	void CloseSocks();
	void Log(std::string sMess);

	std::vector<std::string>  m_vWinFiles; 
};
