#include "linbak.h"
#include "util.h"
#include <iostream>

int main(int argc, char **argv) {
	std::string sPathExtra = "";
	if (argc > 1) {
		sPathExtra +=  argv[1];
		util::SlashEnd(sPathExtra);
	}
	Linbak *pLinbak = new Linbak(sPathExtra);	//sPathRoot and sPathExtra are held in m_sPathRoot and m_sPathExtra
	pLinbak->dirlist();
//	std::cout << "Back from dirlist\n";
	delete pLinbak;
	std::cout << "That's it, leaving.\n";

	//mux.lock();
	std::cout << "Press any key\n";
	//mux.unlock();

	std::cin.ignore();
	return 0;
}
