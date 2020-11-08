#include "linbak.h"
#include "exceptlinbak.h"
#include "util.h"
#include <sstream>
#include <iomanip>

// loop through the file data vector and send names and dates to the server
void Linbak::SendFileNamesAndDates(const std::vector<WIN32_FIND_DATA> &vFileDetails) {
	std::vector<WIN32_FIND_DATA>::const_iterator iterFD;
	WIN32_FIND_DATA nextFileData;
	SYSTEMTIME sysWriteTime;
	memset(&sysWriteTime, 0, sizeof(sysWriteTime));
	for (iterFD = vFileDetails.begin(); iterFD != vFileDetails.end(); ++iterFD) {
		nextFileData = *iterFD;
		//				std::string sMessage = directory + '/' + fileData.cFileName;
		std::string sMessage(nextFileData.cFileName);
		sMessage += '?';
		if (!FileTimeToSystemTime(&nextFileData.ftLastWriteTime, &sysWriteTime)) {
			throw new ExceptLinbak(std::string("Writetime conversion failed for ") + nextFileData.cFileName);
		}
		std::ostringstream osWriteTime;
		osWriteTime << std::setfill('0') << sysWriteTime.wYear << std::setw(2) << sysWriteTime.wMonth;
		osWriteTime << std::setw(2) << sysWriteTime.wDay << std::setw(2) << sysWriteTime.wHour;
		osWriteTime << std::setw(2) << sysWriteTime.wMinute;

		util::exchange(sMessage, "\\\\", "/");
		sMessage += osWriteTime.str();
		m_pSocket->m_SendMux.lock();
//xxx		m_pSocket->Send(sMessage, 'F', ++m_FileSequence);
		m_pSocket->Send(sMessage, 'F');
		m_pSocket->m_SendMux.unlock();
	//	std::cout << sMessage <<'\n';

	}

}