#include "Files.h"
#include <windows.h>

bool Files::fileExists(const std::string &path) const {
	DWORD a = GetFileAttributes(path.c_str());
	return a != (DWORD)-1 && (a & FILE_ATTRIBUTE_DIRECTORY) == 0;
}