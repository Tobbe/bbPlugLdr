#ifndef _FILES_H_
#define _FILES_H_

#include <string>

class Files {
public:
	bool fileExists(const std::string &path) const;
};

#endif