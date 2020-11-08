#include "sync.h"
#include <iostream>
#include "utcpsock.h"
#include <sstream>
#include <iomanip>

//tell client a file needs to be updated
//called by DoUpdates
//vFileNames: names of files to be replaced
//iDSeq: sequence no. of directory
void Sync::RequestUpdates(std::vector<std::string> &vFileNames, unsigned int iDSeq)	{
	unsigned char cSeqL, cSeqR;
	std::ostringstream osFileList(std::stringstream::binary);
	osFileList << std::setfill('0');
	if(vFileNames.size())	{
		cSeqR = iDSeq & 0xff;
		iDSeq >>= 8;
		cSeqL = iDSeq & 0xff;
		for(auto s: vFileNames)	{
			osFileList << std::setw(m_CountWidth) << s.size();
			osFileList << s;
			m_pSocket->Send(osFileList.str().data(),osFileList.str().size(),'R',cSeqL,cSeqR);
			++m_RequestsOutstanding;
			osFileList.str("");
		}
	}
}
