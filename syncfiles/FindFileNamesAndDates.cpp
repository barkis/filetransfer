#include "linbak.h"
#include "util.h"
#include "exceptlinbak.h"

// Look up filenames and dates for files in current directory
void Linbak::FindFileNamesAndDates(const std::string sPath, std::vector<WIN32_FIND_DATA> &vFileDetails,
	std::vector<std::string> &vDirectories) {
	bool bFound = true;
	WIN32_FIND_DATA fileData;
	HANDLE hFile;
	memset(&fileData, 0, sizeof(fileData));

	std::string sGlob(sPath);
	util::exchange(sGlob, "/", "\\");
	hFile = FindFirstFile(sGlob.c_str(), &fileData);
	if (hFile == INVALID_HANDLE_VALUE) {
		throw new ExceptLinbak(std::string("Invalid file handle ") + sGlob);
	}
	else {
		while (bFound) {
			if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {

				std::string dirName(fileData.cFileName);
				//Check if it's one of the excluded directories (dummies like My Music that Microsoft plague us with and ./..)
				int matchCount = count(m_excludedDirectories.begin(), m_excludedDirectories.end(), dirName);
				if (!matchCount) {
					// add directory name to directories vector
					vDirectories.push_back(fileData.cFileName);
				}
			}
			else {
				//add non-directory data to files vector
				vFileDetails.push_back(fileData);
			}
			bFound = FindNextFile(hFile, &fileData);

		}

		int lastError = GetLastError();
		if (lastError != ERROR_NO_MORE_FILES) {
			throw new ExceptLinbak("FindNextFile failed", lastError, __LINE__);
		}
	}

}
