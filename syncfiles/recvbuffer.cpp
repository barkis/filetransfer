#include "tcpsock.h"
#include "recvbuffer.h"
#include <stdexcept>
#include <string>
#include <cstring>
#include <memory>

/*tcpsock::Buffer(tcpsock *pTheSock):m_pSocket(pTheSock),m_RecvDataLength(0),m_Current(0){
	std::cout << "m_Current = " << m_Current <<'\n';
}*/
/*
	Read specified number of chars from buffer, if necessary refilling the buffer from the socket.
	Return value shows whether sufficient chars were available.
*/
int tcpsock::RecvGetNextChars(char *pTo,int len,bool ZTerminate/*=true*/)	{
	bool bRetVal = true;
	//m_SataLength is total length of data in buffer
	//m_RecvCurrent is next value to be read from buffer
	//If more bytes are requested than are available top up the buffer
	if((m_RecvCurrent + len) > m_RecvDataLength)	{
		int iNextPos;
		int iUnread = m_RecvDataLength - m_RecvCurrent;	//no. of bytes unread
		//allocate enough space to hold the unread chars in present block
		std::unique_ptr<char[]>pTempBuff(new char[iUnread]);
		memcpy(pTempBuff.get(),m_RecvData + m_RecvCurrent,iUnread);
		memset(m_RecvData,0,RECVMAXBUFF);
		memcpy(m_RecvData,pTempBuff.get(),iUnread);
		m_RecvCurrent = 0;
		m_RecvDataLength = RecvRead(m_RecvData + iUnread,RECVMAXBUFF - iUnread);
		m_RecvDataLength += iUnread;
		bRetVal = (m_RecvDataLength >= len);
	}
	if(bRetVal)	{
		int i;
		for(i=0;i < len;++i)	{
			*pTo++ = *(m_RecvData + m_RecvCurrent + i);
		}
		m_RecvCurrent += i;
		if(ZTerminate)	{
			*pTo = '\0';
		}
	}
	//return bRetVal;
	return m_RecvDataLength;
}

/* private function to read from socket. Called when buffer runs out
*/
int tcpsock::RecvRead(char *pos,int len)	{
	int iRet = Receive(pos,len);
	if(iRet == -1)	{
		throw std::runtime_error("RecvRead read failed");
	}
	return iRet;
};

int tcpsock::Receive(char *buff, int recvLength) {
	int iReceived;
	if ((iReceived = recv(m_Socket, buff, recvLength, 0)) == SOCKET_ERROR) {
		int iErrno = WSAGetLastError();
		if (iErrno != WSAEWOULDBLOCK) {
			closesocket(m_Socket);
			throw std::runtime_error("Receive failed");
		}
	}
	return iReceived;

}
