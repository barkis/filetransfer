#include "linbak.h"
#include "util.h"
#include <iostream>

extern std::mutex mux;

//When new message is signalled handle it by sending requested file
void Linbak::HandleRequests() {
	MessageTuple mt{};
	int iSeq;
	char cType = '\0';
	std::string sName;
	
	while (cType  != 'E') {
		std::unique_lock<std::mutex> lk(m_pSocket->m_MessageMux);
		//wait till tpsock::ReceiveLoop() signals there's a message in the queue
		m_pSocket->messCond.wait(lk, [=]() {return !m_pSocket->MessageEmpty(); });
		mt = m_pSocket->Message();
		iSeq = std::get<0>(mt);
		cType = std::get<1>(mt);
		sName = std::get<2>(mt);
		lk.unlock();
		
		if (cType == 'R') {
			util::SlashEnd(sName, false);
			util::SlashStart(sName, false);
			mux.lock();
			std::cout << "Handling " << iSeq << ": " << sName << '\n';
			mux.unlock();
			SendFile(iSeq, sName);
		}
	}
	mux.lock();
	std::cout << "leaving HandleRequests\n";
	mux.unlock();
}

