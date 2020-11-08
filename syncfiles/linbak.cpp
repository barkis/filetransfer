#include "linbak.h"
#include "util.h"
#include "exceptlinbak.h"
#include <sstream>
#include <iomanip>
#include <thread>

Linbak::Linbak(std::string sPathExtra) :m_sPathExtra(sPathExtra),
m_DirSequence(1), m_FileSequence(1) {	//seq no.s start at 1 so no seq can be indicated by 0
	try {
		m_excludedDirectories.push_back("My Music");
		m_excludedDirectories.push_back("My Pictures");
		m_excludedDirectories.push_back("My Videos");
		m_excludedDirectories.push_back(".");
		m_excludedDirectories.push_back("..");

		ReadConfig("config.xml");
		m_sPathRoot = getenv("HOMEPATH");
		m_sPathRoot += m_sConfigPath;	// = "/Documents
		util::SlashStart(m_sPathExtra);
		m_sPathRoot += m_sPathExtra;

		m_pSocket = new tcpsock("linux", m_ConfigPort, m_CountWidth);
		m_pSocket->SockSetup();
		util::SlashEnd(m_sPathExtra, false);	//don't want terminating slash
		//send initialisation block
		std::ostringstream os;
		os << m_sPathExtra << '?' << getenv("COMPUTERNAME");
		m_pSocket->Send(os.str(), 'I');	//initialise parameters
		//receive messages from server
		m_pReceiveThread = new std::thread(&tcpsock::ReceiveLoop, m_pSocket);
		//Handle update requests from server
		m_pHandleThread = new std::thread(&Linbak::HandleRequests, this);
	}
	catch (ExceptLinbak* ex) {
		LinbakErrHand(ex);
	}
}

Linbak::~Linbak() {
	m_pSocket->Send("Finished", 'E', (uint16_t)0);
	m_pReceiveThread->join();
//	std::cout << "Back from receive thread\n";
	m_pHandleThread->join();
//	std::cout << "Back from join\n";
	m_pSocket->Closedown();
	delete m_pReceiveThread;
	delete m_pHandleThread;
	delete m_pSocket;
}

