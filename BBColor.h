#ifndef _BBCOLOR_H_
#define _BBCOLOR_H_

#include <windows.h>
#include <string>

class BBColor {
public:
	COLORREF readColorFromString(const std::string &colorstring);
	std::string colorrefToString(COLORREF c);

#ifdef _DEBUG
	friend class UnitTests;
#endif // _DEBUG
private:
	COLORREF parseLiteralColor(const std::string &color);
	std::string &rgbToHexstring(std::string &source);
	COLORREF switchRgb(COLORREF c);
	std::string unquote(const std::string &source);
};

#endif