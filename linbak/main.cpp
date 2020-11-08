#include "sync.h"
#include <stdexcept>
#include <unistd.h>

int main()	{
	Sync *pSync = new Sync;
	try	{
		pSync->MakeSockConnection();
		pSync->MainLoop();
		pSync->CloseSocks();
		std::cout << "That's all folks!" << '\n';
		//sleep(60);
		delete pSync;
	}
	catch(std::runtime_error ex)	{
		pSync->Log(ex.what());
		delete pSync;
	}

}
