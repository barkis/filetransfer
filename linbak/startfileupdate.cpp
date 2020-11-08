#include "sync.h"	//inlcude fstream. string
#include <cstdlib>
#include <unistd.h>
#include <cstring>

//deal with header line of update giving dir sequence and file name
void Sync::StartFileUpdate(char *szFName, unsigned int iDSeq)	{
	if(m_RequestsOutstanding < 0)	{
		throw std::runtime_error("m_RequestsOutstanding < 0");
	}
	auto iter = m_mapSeqDir.find(iDSeq);

	if(iter == m_mapSeqDir.end())	{	//key not in map
		throw std::runtime_error("Key not found in m_mapSeqDir " + std::to_string(iDSeq));
	}
	m_sCurrentDirForUpdate = m_mapSeqDir[iDSeq];
	m_sCurrentFileForUpdate = szFName;
	strcpy(m_szTempFileName,getenv("HOME"));
	strcat(m_szTempFileName,"/temp/XXXXXX");
	int fd = mkstemp(m_szTempFileName);
	if(fd == -1)	{
		std::string sMess("mkstemp() failed for ");
		sMess += m_szTempFileName;
		throw std::runtime_error(sMess);
	}
	close(fd);
	m_TempFileStream.open(m_szTempFileName);
	if(! m_TempFileStream)	{
		m_FileNotOpened = true;
		std::string sMess("Failed to open file in Sync::StartFileUpdate(): ");
		sMess += m_szTempFileName;
		//throw std::runtime_error(sMess);
		Log(sMess);
	}

	
}
