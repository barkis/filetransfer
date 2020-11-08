#include <string>
class tcpsock	{
private:
	std::string m_sServerName;
	int m_Sockfd;
	int m_readSockfd;
	short m_countWidth;
public:
	tcpsock(int portno);
	~tcpsock();
	tcpsock();
	void SockSetup();
	//void Send(std::string message,char cFlag);
	void GetConnection();
	int Receive(char* buff, int recvlength);
	void AcceptError();
	int Send(const std::string sMess, const int size,char cType,unsigned char cSeqL='\0',unsigned char cSeqR='\0');	
	void CloseReadSocket();	
};
