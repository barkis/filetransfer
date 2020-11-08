#include "utcpsock.h"
#include "buffer.h"
#include "sync.h"
#include <stdexcept>
#include <string>
#include <cstring>
#include <memory>

Buffer::Buffer(tcpsock *pTheSock):m_pSocket(pTheSock),m_DataLength(0),m_Current(0){
	std::cout << "m_Current = " << m_Current <<'\n';
}
/*
	Read specified number of chars from buffer, if necessary refilling the buffer from the socket.
	Return value shows whether sufficient chars were available.
*/
int Buffer::GetNextChars(char *pTo,int len,bool zTerminate/*=true*/,Sync *pSync/*=nullptr*/)	{
	bool bRetVal = true;
	//m_DataLength is total lngth of data in buffer
	//m_Current is next value to be read from buffer
	/*if(!m_DataLength)	{
		m_DataLength = read(m_Data,MAXBUFF);
		m_Current = 0;
	}*/
	//If more bytes are requested than are available top up the buffer
	if((m_Current + len) > m_DataLength)	{
		int iNextPos;
		int iUnread = m_DataLength - m_Current;	//no. of bytes unread
		//allocate enough space to hold the unread chars in present block
		std::unique_ptr<char[]>pTempBuff(new char[iUnread]);
		memcpy(pTempBuff.get(),m_Data + m_Current,iUnread);
		memset(m_Data,0,MAXBUFF);
		memcpy(m_Data,pTempBuff.get(),iUnread);
		m_Current = 0;
		m_DataLength = read(m_Data + iUnread,MAXBUFF - iUnread,pSync);
		m_DataLength += iUnread;
		bRetVal = (m_DataLength >= len);
		if(!bRetVal)	{
			std::string sMess("bRetVal false in GetNextChars, m_DataLength = ");
			sMess += std::to_string(m_DataLength);
			sMess += ", len = ";
			sMess += std::to_string(len);
			std::cerr << sMess;
			if(pSync)	{
				pSync->Log(sMess);
				pSync->Log("Data is:");
				pSync->Log(m_Data);
			}
		}
	}
	if(bRetVal)	{
		int i;
		for(i=0;i < len;++i)	{
			*pTo++ = *(m_Data + m_Current + i);
		}
		m_Current += i;
		if(zTerminate)	{
			*pTo = '\0';
		}
	}
	return bRetVal;
	//return m_DataLength;
}

/* private function to read from socket. Called when buffer runs out
*/
int Buffer::read(char *pos,int len,Sync *pSync/*=nullptr*/)	{
	int iTotalReceived = 0, iLoopCount;
	while(iTotalReceived < len)	{
		int iRet = m_pSocket->Receive(pos,len);
		if(iRet == -1)	{
			//throw ReadFailed("Buffer read failed",errno);
			std::cerr << "read failed with "<< errno <<'\n';
		}
		iTotalReceived += iRet;
		if(++iLoopCount > 100)	{
			std::cerr << "not receiving" << '\n';
			break;
		}
	}
	return iTotalReceived;
};

