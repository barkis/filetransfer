#include "sync.h"
#include <cstring>
#include <stdexcept>
#include <memory>

void Sync::read()	{
	 m_pBuff->GetNextChars(&m_CType,1);
	if(!strchr("DFEHUC",m_CType))	{
		std::string sMess("Unexpected m_CType in linbak: ");
		sMess += m_CType;
		throw std::runtime_error(sMess);
	}
	//read sequence no. for file or directory
	if(strchr("DFH",m_CType))	{
		m_Sequence = Sequence(); 
	}
	if(m_CType != 'E')	{
		std::unique_ptr<char[]>pCount(new char[m_CountWidth + 1]);
		char *szCount = pCount.get();
		m_pBuff->GetNextChars(szCount,m_CountWidth);
		szCount[m_CountWidth] = '\0';
		m_NextLen = atoi(szCount);
		m_pBuff->GetNextChars(m_szNext,m_NextLen);
		m_szNext[m_NextLen] = '\0';

	}
	
	
}
