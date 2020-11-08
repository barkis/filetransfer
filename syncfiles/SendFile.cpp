#include "linbak.h"
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <iostream>
#include "exceptlinbak.h"
#include "util.h"
extern std::mutex mux;

void Linbak::SendFile(int iSeq, std::string sName)
{
	const int buffSize = 1000;
	char arrBuff[buffSize];
	std::string sDir = m_mapDirSequence[iSeq];

	//get full file name with backslashes to open the file in Windows
	std::string sFullFileName(m_sPathRoot + sDir + sName);	//sDir ends in '/'
	util::exchange(sFullFileName,"/","\\");
	std::ifstream inFile(sFullFileName.c_str());
	try {

		if (!inFile) {
			std::ostringstream os("Failed to open file ");
			os << sFullFileName << " in SendFile(), faile is ";
			os << std::boolalpha;
			os << inFile.fail();
			os << "bad is ";
			os << inFile.bad();
			os << "eof is ";
			os << inFile.eof();
			std::string sMess = os.str();
			throw std::runtime_error(sMess);
		}
	}
	catch (const std::runtime_error& ex)
	{
		mux.lock();
		std::cerr << ex.what();
		mux.unlock();
	//xxx	exit(1);
	}

	try {
		m_pSocket->m_SendMux.lock();
		m_pSocket->Send(sName, 'H', iSeq);
		mux.lock();
		std::cout << "Sent header " << iSeq << ": " << sName << '\n';
		mux.unlock();


		while (inFile) {
			inFile.read(arrBuff, buffSize);
			m_pSocket->SendUpd(arrBuff, inFile.gcount());
			mux.lock();
			std::cout << "Sent upd " << iSeq << ": " << sName << '\n';
			mux.unlock();
		}
		m_pSocket->Send(sDir + sName, 'C', 0);
		mux.lock();
		std::cout << "Sent completion " << iSeq << ": " << sName << '\n';
		mux.unlock();
		m_pSocket->m_SendMux.unlock();
	}

	catch (ExceptLinbak ex)	{
		std::cerr << ex.what() << '\n';
		inFile.close();
		std::terminate();
	}

}
