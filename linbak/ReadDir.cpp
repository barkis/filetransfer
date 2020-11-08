#include "sync.h"
#include <cassert>
#include <dirent.h>
#include <stdexcept>
/*
   Call GetFiles to read the names and timestamps of files in the specified directory
   Will also create directory if it isn't there
 */

void Sync::ReadDir(char *szNext)	{
	DIR *pDir;
	//std::string sFullDir = sPathRoot + (szNext + COUNTWIDTH + 1);
	std::string sFullDir = m_sPathRoot + szNext;
	if(sFullDir.substr(sFullDir.size()-2) == "//")	{
		sFullDir = sFullDir.substr(0,sFullDir.size() - 1);
	}
	pDir = GetFiles(sFullDir);

	assert(m_vLinFiles.size() == m_vLinDates.size());

	if(pDir)	{
		closedir(pDir);
	}
	else	{
		std::string sMess("DirExistsOrMake failed: ");
		sMess += sFullDir;
		//throw std::runtime_error(sMess);
		Log(sMess);
	}

}
