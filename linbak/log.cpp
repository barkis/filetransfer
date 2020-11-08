#include "sync.h"	//includes <string>

void Sync::Log(std::string sMess)	{
	if(m_LogFile.is_open())	{
		m_LogFile << sMess << '\n';
	}
}
