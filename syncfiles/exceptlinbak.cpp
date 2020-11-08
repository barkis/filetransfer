#include <iostream>
#include <windows.h>
#include <cstdlib>
#include "exceptlinbak.h"
//#include<boost\thread.hpp>
//extern std::mutex mux;
#include <mutex>

extern std::mutex mux;

void LinbakErrHand(ExceptLinbak *ex)	{

	std::cerr << "Linbak exception: \n";
	std::cerr << ex->what() << '\n';
	if(ex->m_iLineno)	{
		std::cerr <<"Line "<<ex->m_iLineno << '\n';
	}
	if(ex->hasErrno())	{
		char szMess[1001];
		if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
					0,
					ex->Errno(),
					0,
					szMess,
					1001,
					NULL))	{
			std::cerr << szMess <<'\n';
		}	
		else	{
			int iErr = GetLastError();
			mux.lock();
			std::cerr << "FormatMessage failed with error "<< iErr << std::hex<<" = "<<iErr<<'\n';
			mux.unlock();
		}
	}
	std::cout << "Press return\n";
	std::cin.ignore();
	exit(1);
}
