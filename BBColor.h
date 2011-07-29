#ifndef _BBCOLOR_H_
#define _BBCOLOR_H_

#include <windows.h>
#include <string>

class BBColor {
public:
	COLORREF readColorFromString(const char *colorstring);
	std::string colorRefToString(COLORREF c);

#ifdef _DEBUG
	friend class UnitTests;
#endif // _DEBUG
private:
	COLORREF parseLiteralColor(const char *color);
	char *rgbToHexstring(char *source);
	COLORREF switchRgb(COLORREF c);
	char *unquote(const char *source, char *dest);
};

#endif