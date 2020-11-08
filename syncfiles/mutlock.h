#pragma once
//#include <boost/thread.hpp>
class mutlock {
private:
	static std::mutex theMutex;
	static bool isLocked;
public:
	static friend std::ostream& operator <<(std::ostream& os, mutlock& ml);
};

std::ostream& operator<<(std::ostream& os, mutlock& ml) {
	if (ml.isLocked) {
		ml.isLocked = false;
		ml.theMutex.unlock();
	}
	else {
		ml.theMutex.lock();
		ml.isLocked = true;
	}
	return os;
}
