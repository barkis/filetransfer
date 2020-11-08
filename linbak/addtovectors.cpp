#include "sync.h"
#include <stdexcept>
#include <cstring>

void Sync::AddToVectors(char *szFileDetails)	{

	m_vWinFiles.push_back(std::strtok(szFileDetails,"?"));
	char *szTime = std::strtok(NULL,"?");
	if(!szTime)	{
		std::string sMess("time not found: ");
		sMess += szFileDetails;
		throw std::runtime_error(sMess);
	}
	else	{
		m_vWinDates.push_back(szTime);
	}
}
