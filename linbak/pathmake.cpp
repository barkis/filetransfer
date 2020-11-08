#include "sync.h"
#include "util.h"
#include <sys/stat.h>
#include <string.h>

//try to open each subpath of the path given and create directories that don't exist
DIR* Sync::PathMake(std::string DirName)	{
	std::vector<std::string> vsDirs = util::split(DirName,'/');
	std::string sPath("");
	bool bSuccess = true;
	DIR *pRetVal = nullptr;
	DIR *pNextDir;
	char szError[1001];
	for(std::string sNext: vsDirs)	{
		sPath += '/' + sNext;
		pNextDir = opendir(DirName.c_str());

		if(!pNextDir)	{
			int errnum = errno;
			if(errnum == ENOENT)	{
				std::cerr << "No ent for " << sPath << '\n';
			
				if(mkdir(sPath.c_str(),0755))	{
					if(errno != EEXIST)	{
						perror("mkdir");
						bSuccess = false;
						Log(strerror(errno));		
						break;
					}
					else	{
						std::cerr << "Dir exists "<<sPath << '\n';
					}
				}
			}
			else	{
				std::cerr << "opendir failed for " << errnum << '\n';
			}
		}
		else	{
			std::cout << "Closing " << pNextDir << '\n';
			closedir(pNextDir);
		}
	}
	if(bSuccess)	{
		pRetVal = opendir(DirName.c_str());
		if(pRetVal)	{
			std::cout << "Opened "<< DirName << '\n';
		}
		else	{
			std::cout << "Failed to open "<< DirName << '\n';
			
		}
	}
	return pRetVal;
}
