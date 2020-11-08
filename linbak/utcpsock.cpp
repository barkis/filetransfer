#include "utcpsock.h"
#include "util.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <errno.h>
#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <stdexcept>
#include <algorithm>

tcpsock::tcpsock(int portno):m_Sockfd(0),m_readSockfd(0)	{
	sockaddr_in addr;

	m_Sockfd = socket(AF_INET,SOCK_STREAM,0);
	int iErr;
	short iTrue = 1;
	setsockopt(m_Sockfd,SOL_SOCKET,SO_REUSEADDR,&iTrue,sizeof(short));
	memset(&addr,0,sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(portno);
	if(iErr = bind(m_Sockfd,(const sockaddr*)&addr,sizeof(addr))<0)	{
		perror("Bind failed");
		exit(1);
	}	
}

void tcpsock::GetConnection()	{
	sockaddr addrFrom;
	struct sockaddr_in servaddr,cli_addr;
	socklen_t clilen;
	unsigned int addrSize = sizeof(addrFrom);
	listen(m_Sockfd,5);
	clilen = sizeof(cli_addr);
	m_readSockfd = accept(m_Sockfd,(struct sockaddr*)&cli_addr,&clilen);
	if(m_readSockfd == -1)	{
		AcceptError();
		exit(1);
	}	
	else	{
		std::cerr << "Accepted\n";
	}
}

void tcpsock::CloseReadSocket()	{
	close(m_readSockfd);
	m_readSockfd = 0;
}

tcpsock::~tcpsock()	{
	if(m_Sockfd)	{
		close(m_Sockfd);
	}
	if(m_readSockfd)	{
		close(m_readSockfd);
	}
}

int tcpsock::Receive(char* buff,int recvlength)	{
	int n = recv(m_readSockfd,buff,recvlength,0);	
//	std::cerr << "Received " << n << " bytes \n";
	return n;
}

void tcpsock::AcceptError()	{
	std::cerr << "accept failed\n";
	switch(errno)	{
		case EBADF: 
			std::cerr << "Descriptor invalid\n";
			break;
		case EFAULT:
			std::cerr << "addr parameter not writable part of user address space\n";
			break;
		case ENOTSOCK:
			std::cerr << "Descriptor references a file\n";
			break;
		case EOPNOTSUPP:
			std::cerr << "References socket not of type SOCK_STREAM\n";
			break;
		case EWOULDBLOCK:
			std::cerr << "marked non-blocking and no connections\n";
			break;
		default:
			std::cerr << "unknown error "<<errno<<'\n';
	}

}
/* On the established TCP connection send cType, the 2 sequence bytes if present, the message
The message starts with the size (in COUNTWIDTH bytes)
 */
int tcpsock::Send(const std::string sMess, const int size,char cType,unsigned char cSeqL/*='\0'*/,unsigned char cSeqR/*='\0'*/)	{
	unsigned int iInc = 0;
	if(cSeqL | cSeqR)	{
		iInc = 2;
	}
	util::Array<char>arrMess(size + iInc + 1);
	arrMess[0] = cType;
	if(iInc)	{
		arrMess[1] = cSeqL;
		arrMess[2] = cSeqR;
	}
	//use STL copy on arrays. Inp first=sMess.data(),Inp last=1 past end of sMess, Out result=arrMess beyond cType [and seq bytes]
	arrMess.copy(const_cast<char*>(sMess.data()),const_cast<char*>(sMess.data()+size),1 + iInc);

	//send on socket
	int iSent = send(m_readSockfd, arrMess.data(), size + 1 + iInc, 0);
	//delete[] arrMess;
	if(iSent == -1)	{
		throw std::runtime_error("send failed " + std::to_string(iSent));
	}
	std::cout << "Sent " << iSent << ": " <<  arrMess[0] <<":"<< (int)arrMess[1]<<":" <<(int) arrMess[2]<<":" ;
	std::cout.write(arrMess.data() + 3 ,size);
	std::cout << '\n';
	return iSent == -1?0:iSent;
}
