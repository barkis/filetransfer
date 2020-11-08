//#define mainprog
#include <regex>
#include <iostream>
#include <cstdlib>
#include <vector>
#include "util.h"	//include <string>
/*
 * Replace oldchar with newchar in stringin and return the altered string
*/
void util::exchange(std::string& stringin, const char* oldchar, const char* newchar) {
	std::string sTemp;
	std::regex reg(oldchar);

	std::string sNewChar(newchar);
	sTemp = std::regex_replace(stringin, reg, sNewChar);
	stringin = sTemp;
}

void util::SlashEnd(std::string &stringin,bool set /*=true*/) {
	if (!stringin.size() && set) {
		stringin += '/';
	}
	else {
		if (stringin[stringin.size() - 1] != '/') {
			
			if (set) {
				stringin += '/';
			}
		}
		else if (!set) {
			stringin = stringin.substr(0, stringin.size() - 1);
		}
	}
}

void util::SlashStart(std::string &stringin, bool set /*=true*/) {
	if (!stringin.size() && set) {
		stringin += '/';
	}
	else {
		if (stringin[0] != '/') {

			if (set) {
				std::string sTemp(1, '/');
				stringin = sTemp + stringin;
			}
		}
		else if (!set) {
			stringin = stringin.substr(1);
		}
	}
}

std::vector<std::string> util::split(std::string strin, char delim/* = '\04'*/) {
	std::string::size_type notFound = std::string::npos;
	std::vector<std::string> vsParts;
	std::string::size_type nextPos = 0, furtherPos;


	int start = 0;
	size_t next = 0;

	while (next != notFound) {
		next = strin.find(delim, start);

		std::string sNext = strin.substr(start, next - start);
		if (sNext.size()) {
			vsParts.push_back(sNext);
		}
		if (next != notFound) {
			start = next + 1;
		}
	}
	return vsParts;
}
#ifdef mainprog
int main() {
	std::string noslash = "here";
	std::string withslash("/there");
	util::SlashStart(noslash);
	util::SlashStart(withslash, false);
	std::cout << "noslash = " << noslash << ", withslash = " << withslash << '\n';
}
#endif

//from Stroustrup p. 299
template<class Target, class Source>
Target util::narrow_cast(Source v) {
	auto r = static_cast<Target>(v);
	if (static_cast<Source>(r) != v) {
		throw std::runtime_error("narrow_cast<>() failed");
	}
	return r;
}

