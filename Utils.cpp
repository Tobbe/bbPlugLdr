#include "Utils.h"
#include <windows.h>

bool Utils::fileExists(const std::string &path) {
	DWORD a = GetFileAttributes(path.c_str());
	return a != (DWORD)-1 && (a & FILE_ATTRIBUTE_DIRECTORY) == 0;
}