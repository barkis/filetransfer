#include "tcpsock.h"
#include <Ws2tcpip.h>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <assert.h>
#include <iostream>
#include <memory>
//#include <boost\thread.hpp>
#include <thread>
#include "exceptlinbak.h"
#include "mutlock.h"
#include "util.h"
#include <cstring>
#include <mutex>
extern std::mutex mux;

tcpsock::tcpsock(std::string sServerName, int Port, short CountWidth) : m_sServerName(sServerName), m_Port(Port),
m_CountWidth(CountWidth), m_bNonBlocking(false), m_RecvCurrent(0), m_RecvDataLength(0) {}

void tcpsock::SockSetup() {
	WSADATA              wsaData;
	SOCKADDR_IN          ServerAddr;
	int                  Ret;


	std::ostringstream os;
	if ((Ret = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		throw new ExceptLinbak("WSAStartup failed ");
	}


	// Create a new socket to make a client connection.

	if ((m_Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))
		== INVALID_SOCKET)
	{
		int iErrno = WSAGetLastError();
		Closedown();
		throw  new ExceptLinbak("socket ctor failed ", iErrno);
	}

	// Set up a SOCKADDR_IN structure that will be used to connect
	// to a listening server 
	addrinfo *result;
	if (int error = getaddrinfo(m_sServerName.c_str(), NULL, NULL, &result)) {
		mux.lock();
		std::cerr << "getaddrinfo failed with " << error << '\n';
		mux.unlock();

		exit(1);
	}
	sockaddr *sa = result->ai_addr;
	sockaddr_in *sin = (sockaddr_in*)(sa);
	unsigned long iIPAddr = sin->sin_addr.s_addr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(m_Port);
	ServerAddr.sin_addr.s_addr = iIPAddr;

	if (connect(m_Socket, (SOCKADDR *)&ServerAddr, sizeof(ServerAddr))
		== SOCKET_ERROR)
	{
		int iErrno = WSAGetLastError();
		Closedown();
		throw new ExceptLinbak(os.str().c_str(), iErrno);
	}
}

/*
	Send block of data from file being updated
	buff: start of block
	size: size of block
	*/
void tcpsock::SendUpd(char *buff, long long size) {
	char cFlag = 'U';
	int bytesSent;
	util::Array<char> arrBuff(size + 1 + m_CountWidth);
	//put flag at start
	arrBuff[0] = cFlag;
	std::ostringstream os("");
	//follow flag with size of block in buffer to be sent
	os << std::setfill('0') << std::setw(m_CountWidth) << size;
	std::string sSize = os.str();
	const char *pData = sSize.data();

	arrBuff.copy(const_cast<char*>(pData), const_cast<char*>(pData + m_CountWidth), 1);
	//add block to buffer to be sent
	arrBuff.copy(buff, buff + size, 1 + m_CountWidth);
	//std::copy(buff, buff + size, &arrBuff[1 + m_CountWidth]);
	//send the buffer
	if (arrBuff[0] != 'U')	{
		throw ExceptLinbak("no U prefix in SendUpd");
	}
	int iBytesToSend = size + 1 + m_CountWidth;
	bytesSent = send(m_Socket, arrBuff.data(), iBytesToSend, 0);
	if (bytesSent == SOCKET_ERROR)	{
		int iErrno = WSAGetLastError();
		Closedown();
		std::string sMess("Send failed in block Send(): ");
		throw ExceptLinbak(sMess, iErrno, __LINE__);
	}
	if (bytesSent != iBytesToSend)	{
		std::string sMess("Send shortfall, btes to send:");
		sMess += std::to_string(iBytesToSend);
		sMess += ", Bytes sent: ";
		sMess += std::to_string(bytesSent);
		throw ExceptLinbak(sMess);
	}
}

void tcpsock::Send(std::string message, char cFlag, std::uint16_t seq/* = 0*/) {

	int bytesSent = 0, totalSent = 0;
	bool bWouldBlock;
	short iWouldBlockCount;
	std::ostringstream os;
	unsigned char cSeqLB{}, cSeqRB{};

	bool bIsSeq = seq;
	//os << cFlag;
	try
	{
		if (bIsSeq) {
			int iSeqCopy = seq;
			cSeqRB = iSeqCopy & 0xff;	//right 8 bits of sequence number
			iSeqCopy &= 0xff00;
			iSeqCopy >>= 8;			//left 8 bits moved to right
			cSeqLB = iSeqCopy & 0xff;
			//	os << cSeqLB << cSeqRB;
		}

		int messLen = message.length();
		int sendLen = 1 + messLen + m_CountWidth + (bIsSeq ? 2 : 0);
		util::Array<char> arrToSend(sendLen);
		arrToSend[0] = cFlag;
		if (bIsSeq) {
			arrToSend[1] = cSeqLB;
			arrToSend[2] = cSeqRB;
		}


		os << std::setw(m_CountWidth) << std::setfill('0') << messLen << std::setw(0) << message;

		std::string fullMessage = os.str();
		//Copy the message length followed by the message to the place after the type code and after the sequence bytes where aprropriate
		arrToSend.copy(const_cast<char*>(fullMessage.data()), const_cast<char*>(fullMessage.data() + fullMessage.size()), 1 + (bIsSeq ? 2 : 0));
		while (totalSent < messLen + m_CountWidth) {
			bWouldBlock = false;
			iWouldBlockCount = 0;


			do {
				//send cFlag cSeqLB cSeqRB messLen Message
				if ((bytesSent = send(m_Socket, arrToSend.data(), sendLen, 0)) == SOCKET_ERROR)
				{
					int iErrno = WSAGetLastError();
					if (iErrno == WSAEWOULDBLOCK) {
						mux.lock();
						std::cerr << "Send would block\n";
						mux.unlock();

						bWouldBlock = true;
						if (++iWouldBlockCount > 100) {
							Closedown();
							throw ExceptLinbak("Repeating WSAEWOULDBLOCK in Send ");
						}
					}
					else {

						Closedown();
						std::string sMess("Send failed: ");
						sMess += fullMessage;
						throw ExceptLinbak(sMess, iErrno, __LINE__);
					}
				}
				else if (bytesSent < sendLen)	{
					char szMess[101];
					sprintf(szMess, "sent less than requested, bytes sent = %d, sendLen = %d", bytesSent, sendLen);
					throw ExceptLinbak(szMess);
				}
			} while (bWouldBlock);	//repeat if there's a WouldBlock error
			totalSent += bytesSent;
		}	//end outer loop
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << '\n';
		std::cout << "Press return\n";
		std::cin.ignore();
		std::exit(1);
	}

}


void tcpsock::SendMinusOne() {
	mux.lock();
	std::cout << "SENDMINUSONE\n";
	mux.unlock();

	std::ostringstream os;
	os << std::setw(m_CountWidth) << -1;
	int sent = send(m_Socket, os.str().c_str(), m_CountWidth, 0);
	if (sent != m_CountWidth) {
		int iErrno = WSAGetLastError();
		Closedown();
		throw new ExceptLinbak("SendMinusOne failed ", iErrno);
	}
}



void tcpsock::ReceiveLoop() {
	char arrRecv[1001];
	int iReceived;
	int iTotal;
	int iRemainder{};
	uint16_t iSeq{};
	std::string sReceived("");
	std::unique_ptr<char[]> pCount(new char[m_CountWidth + 1]);
	char *szCount = pCount.get();
	std::string sEnd(m_CountWidth, '9');
	int iEnd = atoi(sEnd.c_str());
	int iCount;
	char cType;
	const int recvlength = 1000;
	char aiSeq[2];
	try {
		do
		{
			iSeq = 0;
			//R[equest] line is 'R', seqL, seqR, size, message
			//cType = arrRecv[iTotal++];
			if (!RecvGetNextChars(&cType, 1, false)) {
				throw std::runtime_error("No receive in ReceiveLoop");
			}
			if (cType != 'E' && cType != 'R') {
				throw std::runtime_error("Invalid cType in ReceiveLoop");
			}
			if (cType == 'R') {
				RecvGetNextChars(aiSeq, 2, false);
				unsigned char cL = aiSeq[0];
				unsigned char cR = aiSeq[1];
				iSeq = cL;
				iSeq <<= 8;
				iSeq |= cR;
			}
			//Get length of message
			std::memset(szCount, 0, m_CountWidth + 1);
			RecvGetNextChars(szCount, m_CountWidth);
			iCount = std::atoi(szCount);
			//xxxif (iCount == 0 || iCount == iEnd) {
			//	break;
			//}

			if (cType == 'E') {
				sReceived = "9999";
				iSeq = 0;
				mux.lock();
				std::cout << "End in HandleRequests\n ";
				mux.unlock();
			}
			else {
				RecvGetNextChars(arrRecv, iCount, true);
				sReceived.assign(arrRecv);
				mux.lock();
				std::cout << "Received " << sReceived << '\n';
				mux.unlock();
			}
			MessageTuple mp{ iSeq, cType, sReceived };
			m_MessageMux.lock();
			qMessages.push(mp);
			//tell HandleRequests() there's a request waiting
			messCond.notify_one();
			m_MessageMux.unlock();


		} while (cType != 'E');
	}
	catch (ExceptLinbak ex) {
		std::cout << ex.what() << '\n';
	}
	std::cout << "Receive ended\n";
}

void tcpsock::Closedown() {
	std::cout << "Closedown\n";

	if (m_Socket) {
		linger structLinger;
		structLinger.l_onoff = 1;
		structLinger.l_linger = 5000;
		Sleep(1000);
		setsockopt(m_Socket, SOL_SOCKET, SO_LINGER, (char*)&structLinger, sizeof(structLinger));
		//shutdown(m_Socket,SD_BOTH);
		closesocket(m_Socket);
	}
	WSACleanup();
}

bool tcpsock::SetNonBlocking(bool setNB/* = true*/) {
	unsigned long ul = setNB;

	int iRet = ioctlsocket(m_Socket, FIONBIO, &ul);
	if (iRet == SOCKET_ERROR) {
		std::string sMess = "SetNonBlocking failed " + ioctlError();
		throw new ExceptLinbak(sMess, iRet);
		return false;
	}
	else {
		m_bNonBlocking = true;
		return true;
	}
	m_bNonBlocking = true;
}

MessageTuple tcpsock::Message()
{
	MessageTuple RetVal{ 0, '\0', "" };
	if (qMessages.size()) {
		RetVal = qMessages.front();
		qMessages.pop();
	}
	return RetVal;
}

bool tcpsock::MessageEmpty() {
	return !qMessages.size();
}

std::string tcpsock::ioctlError() {
	std::string sRetVal;

	int iErrNo = WSAGetLastError();

	switch (iErrNo) {
	case  WSANOTINITIALISED:
		sRetVal = "A successful WSAStartup must occur before using this function.";
		break;
	case WSAENETDOWN:
		sRetVal = "The network subsystem has failed.";
		break;
	case WSAEINPROGRESS:
		sRetVal = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";
		break;
	case WSAENOTSOCK:
		sRetVal = "The descriptor s is not a socket.";
		break;
	case WSAEFAULT:
		sRetVal = "The argp argument is not a valid part of the user address space.";
		break;
	case WSAEOPNOTSUPP:
		sRetVal = "Operation not supported.";
		break;
	default:
		mux.lock();
		std::cerr << "Error: " << iErrNo << std::hex << iErrNo << '\n';
		mux.unlock();

		sRetVal = "Unknown error: " + iErrNo;
	}
	return sRetVal;
}
