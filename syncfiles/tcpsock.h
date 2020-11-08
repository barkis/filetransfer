#define __USE_W32_SOCKETS
#include <winsock2.h>
#include <vector>
#include <string>
#include <cstdint>
#include <queue>
//#include <boost/thread.hpp>
#include <thread>
#include <tuple>
#include <mutex>
using MessageTuple = std::tuple<int, char, std::string>;
class tcpsock	{
private:
	std::string m_sServerName;
	SOCKET m_Socket;
	short m_countWidth;
	bool m_bNonBlocking;
	int m_Port;
	short m_CountWidth;
	unsigned int m_RecvCurrent;
	unsigned int m_RecvDataLength;
	static const int RECVMAXBUFF = 1000;
	char m_RecvData[RECVMAXBUFF];

	std::string ioctlError();
	std::queue<MessageTuple>qMessages;
	int RecvGetNextChars(char *pTo, int len, bool ZTerminate=true);
	int RecvRead(char *pos, int len);
public:
	tcpsock(std::string sServerName,int Port,short CountWidth);
	std::mutex m_MessageMux;
	std::mutex m_SendMux;
	std::condition_variable messCond;
	//tcpsock(std::vector<int>  &theIpaddr): m_ipaddr(theIpaddr),m_Socket(0){} 
	void SockSetup();
	void Send(std::string message,char cFlag, std::uint16_t seq=0);
	void SendUpd(char *buff,long long size);
	void ReceiveLoop();
	int Receive(char *buff, int recvLength);
	void Closedown();
	void SendMinusOne();
	bool SetNonBlocking(bool setNB = true);
	MessageTuple Message();
	bool MessageEmpty();
};
