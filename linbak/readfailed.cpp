#include "sync.h"
const char* ReadFailed::what()	{
	std::string sRet(m_sText);
	sRet += ": ";
	sRet += std::to_string(m_Code);
	return sRet.c_str();
}
