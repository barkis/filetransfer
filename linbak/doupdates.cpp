#include "sync.h"
#include <iostream>
#include <algorithm>

/* DoUpdates Called by MainLoop at end of each directory and of transmission
szNext: full directory name
iDSeq: directory sequence no. to identify directory to the client
*/
void Sync::DoUpdates(char *szNext,unsigned int iDSeq)	{
	//First handle the files read from the previous directory

	if(m_vWinFiles.size())	{
		//get list of files that need updating
		std::vector<std::string> vsUpdate = GetDiff();
		
		if(vsUpdate.size())	{
			RequestUpdates(vsUpdate,iDSeq);
		}
	}
	m_vLinDates.clear();
	m_vLinFiles.clear();
	m_vWinFiles.clear();
	m_vWinDates.clear();
//	ReadDir(szNext);
}

