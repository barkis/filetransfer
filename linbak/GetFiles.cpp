//GetFiles.cpp
#include "sync.h"	//includes dirent.h, string
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctime>
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>

	
/*
	Called by DoUpdates()
	Read through the given directory getting file names and date information
	Calls DirExistsOrMake to make directories on path that don't exist
	Then adds filenames and mod dates to m_vLinFiles and m_vLinDates
 */
DIR* Sync::GetFiles(std::string sDirPath)	{
	dirent *pEntry;
	DIR *pDir;
	struct stat statStruct;
	struct tm *pTimeStruct;
	time_t modTime;
	std::ostringstream osStream;

	std::string sFullName;
	std::string sModTime;
	if(pDir = DirExistsOrMake(sDirPath))	{
		while(pEntry = readdir(pDir))	{
			if(strcmp(pEntry->d_name,".") && strcmp(pEntry->d_name,".."))	{
				sFullName = sDirPath + '/';
				sFullName += pEntry->d_name;
				//fill statStruct with information about the file
				if(int iRet = stat(sFullName.c_str(),&statStruct))	{
					throw std::runtime_error("stat failed in GetFiles");
				}
				int reg = S_IFREG;
				int fmt = S_IFMT;
				int anded = statStruct.st_mode & S_IFMT;
				if((statStruct.st_mode  & fmt) == reg)	{	//is a regular file
					modTime = statStruct.st_mtime;
					pTimeStruct = gmtime(&modTime);
					osStream.fill('0');
					int iRealYear = pTimeStruct->tm_year + 1900;
					osStream << iRealYear << std::setw(2)<< pTimeStruct->tm_mon + 1 << std::setw(2) << pTimeStruct->tm_mday; 
					osStream << std::setw(2) << pTimeStruct->tm_hour << std::setw(2) << pTimeStruct->tm_min ;
					m_vLinFiles.push_back(std::string(pEntry->d_name));
					m_vLinDates.push_back(osStream.str());
					//xxx
					std::cout << pEntry->d_name << '\n';
					osStream.str("");
				}
			}
		}
	}
	else	{
		std::cerr << "DirExistsOrMake failed: " << sDirPath << '\n';
	}
	return pDir;
}
