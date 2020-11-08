#include "sync.h"
#include <stdexcept>
#include <cstring>
#include <memory>

void Sync::Initialise(std::string sInitBlock)	{
	std::string::size_type qPos = sInitBlock.find('?');
	std::string sClientRootPath = sInitBlock.substr(0,qPos);
	m_sClientMachine = sInitBlock.substr(qPos + 1);
	m_sPathRoot = getenv("HOME");
	m_sPathRoot += m_sConfigPath;
	m_sPathRoot += '/';
	m_sPathRoot += m_sClientMachine;
	m_sPathRoot += sClientRootPath;
}
