#include "sync.h"	//include dirent.h, string
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

/*
   If the directory doesn't exist create it
   Open the directory and return a pointer to it
*/
DIR * Sync::DirExistsOrMake(std::string DirName)	{

	char szError[1001];
	DIR *pRetVal = nullptr;
	pRetVal = opendir(DirName.c_str());
	if(!pRetVal)		{
		int iErr = errno;
		sprintf(szError,"opendir failed for %s with %d: %s",DirName.c_str(),errno,strerror(errno));
		Log(szError);
		if(!mkdir(DirName.c_str(),0755))	{	//mkdir returns 0 on success
			pRetVal = opendir(DirName.c_str());
			if(pRetVal)	{
				sprintf(szError,"opened %s",DirName.c_str());
				Log(szError);
			}
		}
		else	{
			pRetVal = PathMake(DirName);
			if(!pRetVal)	{
				sprintf(szError,"PathMake failed for %s",DirName);
				Log(szError);
			}
		}
	}
	return pRetVal;
}

