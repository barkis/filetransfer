#include "utcpsock.h"	//includes std::string
#include "buffer.h"
#include <cassert>
#include <iomanip>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <stdexcept>
#include <algorithm>
#include <memory>
#include "sync.h"


void Sync::MainLoop()	{
	DIR *pDir = nullptr;
	std::string sEnd(m_CountWidth,'9');
	//	std::cout << "Receiving\n";
	ssize_t n;
	int iSendCount = 0;
	m_RequestsOutstanding = 0;
	m_EndReceived = false;
	std::ostringstream os;
	int iDebugCount = 0;
	std::string sClientRootPath;
	unsigned int iDSeq = 0, iFSeq;
	unsigned int iOldDSeq;
	unsigned int iSequence;
	int iDataFound ;
	char szNext[1000];
	int iTotal;
	int iCount;
	int iLoopCount = 0;	//xxx
	int iLoopsPastEnd = 0;
	std::unique_ptr<char[]>pCount(new char[m_CountWidth + 1]);
	char *szCount = pCount.get();
	// read initial batch which may contain 1 or more entries starting with initialisation block. 
	//Format type(char: 'I'nitialise, 'D'irectory, 'F'ile or 'E'nd), length(integer{m_CountWidth}) '@' fullname modtime(yyyymmddhhmmm)
	//Buffer buff(m_pSocket);
	//n = m_pSocket->Receive(buff + iOrphanLen,BUFSIZE - m_m_CountWidth -1);
	//outer loop filling the buffer with each iteration
	//batch may contain 1 or more entries. If it isn't the first time this may start with the continuation of an entry
	//Format: type(character,'D','F' or 'E') length(integer of length m_CountWidth '@' string fullname string modtime(yyyymmddhhmm)
	do {
		iLoopCount++;
		if(m_pBuff->GetNextChars(&m_cType,1,false))	{
			//read sequence no. for file or directory
			if(strchr("DH",m_cType))	{
				char acSeq[2];
				if(!m_pBuff->GetNextChars(acSeq,2,false))	{
					throw std::runtime_error("failed to read acSeq");
				}
				unsigned char aucSeq[2];
				aucSeq[0] = (unsigned char)acSeq[0];
				aucSeq[1] = (unsigned char)acSeq[1];
				iSequence = Sequence(aucSeq); 
			}

			if(!m_pBuff->GetNextChars(szCount,m_CountWidth))	{
				throw std::runtime_error("Failed to read szCount");
			}
			szCount[m_CountWidth] = '\0';
			iCount = std::atoi(szCount);
			if(!m_pBuff->GetNextChars(szNext,iCount,true,this))	{
				throw std::runtime_error("failed to read szNext");
			}
			//take action depending on the type code
			switch(m_cType)	{
				case 'F':
					//iFSeq = iSequence;
					// add file name and mod time to vectors
					AddToVectors(szNext);
					break;
				case 'D':
				case 'E':	//end of a directory so process the filenames received
					//First find any files that need updating using the old directory id.
					DoUpdates(szNext,iDSeq);
					iDSeq = iSequence;
					if(m_cType == 'D')	{
					// Record directory seq and name pairing for use in subsequent file updates
						m_mapSeqDir.insert(std::make_pair(iDSeq,szNext));
					}
					if(m_cType == 'E')	{
						m_EndReceived = true;
					}
					else	{
						ReadDir(szNext);
					}
					break;
				case 'H':	
					//start of send of file from client
					StartFileUpdate(szNext,iSequence);
					break;
				case'U':
					//next block of file being updated
					//ignore if file open failed
					if(!m_FileNotOpened)	{
						m_TempFileStream.write(szNext,iCount);
					}
					break;
				case 'C':
					//end of send of file from client
					m_FileNotOpened = false;
					EndFileUpdate();
					break;
				case 'I':
					Initialise(szNext);
					break;
				default:
					std::string sMess("Unexpected m_cType in MainLoop: ");
					sMess += m_cType;
					if(m_cType == ' ')	{
						sMess += "(space)";
					}	
					Log(sMess);
					Log(m_pBuff->dump());
					throw std::runtime_error(sMess);
					break;
			}
			if(m_EndReceived)	{
				if(m_RequestsOutstanding == 0)	{
					break;
				}
			}
		}	//if ctype found
		else	{
			std::string sMess("Failed to read m_cType in MainLoop: ");
			Log(sMess);
			Log(m_pBuff->dump());
			throw std::runtime_error(sMess);

		}
	}while(true);	//end outer loop
	m_pSocket->Send(sEnd,m_CountWidth + 1,'E'); 
	std::cout << "\n sent " << sEnd << '\n';
}





