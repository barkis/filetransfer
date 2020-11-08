#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cassert>
//#include<boost\thread.hpp>
#include "util.h"
#include "exceptlinbak.h"
#include "linbak.h"


std::mutex mux;
/*
	Send directory name and names of files contained with timestamps to server.
	Then call dirlist recursively with names of subdirectories
*/
void Linbak::dirlist(std::string directory/*=""*/, int depth/*=0*/) {
	std::vector<std::string>vDirectories;
	std::vector<WIN32_FIND_DATA>vFiles;
	std::string sRootlessFileName;
	std::string sRootlessDir;
	try {

		util::SlashEnd(directory);
		//Associate the current dir with a sequence no.
		m_mapDirSequence.insert(std::make_pair(m_DirSequence, directory));
		//Send current dir message to server
		m_pSocket->Send(directory, 'D', m_DirSequence++);
	//	util::SlashStart(directory, false);
		std::string sPath = m_sPathRoot + directory;
		sPath += "*";	//directory will be search root on 1st call, deeper on recursive calls
		FindFileNamesAndDates(sPath, vFiles, vDirectories);

		//iterate through the file data structures for this directory and send them
		SendFileNamesAndDates(vFiles);

		//Having done the files recurse through the subdirectories
		std::vector<std::string>::iterator iter;
		for (iter = vDirectories.begin(); iter != vDirectories.end(); ++iter) {
			//Get rid of the directories above the root of the directory structure from which the syncronisations starts
			std::string sDirMessage = directory;
			sDirMessage += *iter;
			mux.lock();
		//	std::cout << sDirMessage << m_DirSequence << '\n';
			mux.unlock();
			dirlist(sDirMessage, depth + 1);
		}
	}
	catch (ExceptLinbak *ex) {
		m_pSocket->Closedown();
		LinbakErrHand(ex);
		exit(1);
	}
}


