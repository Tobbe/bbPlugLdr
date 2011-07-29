#ifndef _MESSAGE_MAP_
#define _MESSAGE_MAP_

#include <map>
#include <windows.h>

class MessageMap {
public:
	MessageMap();
	~MessageMap();

	void addMessages(const unsigned int* const messages, HWND hWnd);
	bool removeMessages(const unsigned int* const messages, HWND hWnd);
	void sendMessage(unsigned int message, WPARAM wParam, LPARAM lParam) const;

#ifdef _DEBUG
	friend class UnitTests;
#endif // _DEBUG
private:
	std::multimap<unsigned int, HWND> msgmap;
};
#endif // _MESSAGE_MAP_