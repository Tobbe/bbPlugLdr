#ifndef _BBRNR_H_
#define _BBRNR_H_

#define BB_REGISTERMESSAGE      10001
#define BB_UNREGISTERMESSAGE    10002
#define BB_EXECUTE              10202 /* Send a command or bro@m for execution (in lParam) */
#define BB_BROADCAST            10901 /* lParam: command string */

// Gradient types + solid
#define B_HORIZONTAL        0
#define B_VERTICAL          1
#define B_DIAGONAL          2
#define B_CROSSDIAGONAL     3
#define B_PIPECROSS         4
#define B_ELLIPTIC          5
#define B_RECTANGLE         6
#define B_PYRAMID           7
#define B_SOLID             8
#define B_SPLITVERTICAL     9
#define B_MIRRORHORIZONTAL  10
#define B_MIRRORVERTICAL    11

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

class Menu;
class MenuItem;
class StyleItem;
class SystemTray;

extern "C" {	
	__declspec(dllexport) int initModuleEx(HWND hwndParent, HINSTANCE hInstance, LPCSTR szPath);
	__declspec(dllexport) void quitModule(HINSTANCE hInstance);

	__declspec(dllexport) HWND GetBBWnd();
	__declspec(dllexport) const char* const GetBBVersion();

	__declspec(dllexport) void GetBlackboxEditor(char *editor);
	__declspec(dllexport) char * WINAPI GetBlackboxPath(char *path, int maxLength);
	__declspec(dllexport) const char *stylePath(const char styleFileName[] = NULL);

	__declspec(dllexport) bool BBExecute(HWND owner, const char szOperation[], const char szCommand[], const char szArgs[], const char szDirectory[], int nShowCmd, bool noErrorMsgs);

	__declspec(dllexport) bool ReadBool(const char filePath[], const char key[], bool defaultBool);
	__declspec(dllexport) int ReadInt(const char filePath[], const char key[], int defaultInt);
	__declspec(dllexport) const char* const ReadString(const char filePath[], const char key[], const char defaultString[]);
	__declspec(dllexport) COLORREF ReadColor(const char filePath[], const char key[], const char defaultString[]);

	__declspec(dllexport) void WriteBool(const char filePath[], const char key[], bool value);
	__declspec(dllexport) void WriteInt(const char filePath[], const char key[], int value);
	__declspec(dllexport) void WriteString(const char filePath[], const char key[], const char value[]);
	__declspec(dllexport) void WriteColor(const char filePath[], const char key[], COLORREF value);

	__declspec(dllexport) bool FileExists(const char filePath[]);

	__declspec(dllexport) void SnapWindowToEdge(WINDOWPOS* windowPosition, LPARAM nDist_or_pContent, unsigned int flags);
/*	__declspec(dllexport) bool SetTransparency(HWND hwnd, BYTE alpha);

	__declspec(dllexport) void MakeSticky(HWND window); // Make a window visible on all workspaces
	__declspec(dllexport) void RemoveSticky(HWND window);
	__declspec(dllexport) bool CheckSticky(HWND window);*/

	__declspec(dllexport) void ShowMenu(Menu *pluginMenu);
	//__declspec(dllexport) Menu *MakeNamedMenu(const char headerText[], const char id[], bool fshow);
	__declspec(dllexport) MenuItem *MakeMenuItem(Menu *pluginMenu, const char title[], const char cmd[], bool showIndicator);
	__declspec(dllexport) MenuItem *MakeMenuNOP(Menu *pluginMenu, const char title[] = NULL);
	//__declspec(dllexport) MenuItem *MakeMenuItemString(Menu *pluginMenu, const char title[], const char cmd[], const char initString[] = NULL); // inserts an item to edit a string value
	//__declspec(dllexport) MenuItem *MakeMenuItemInt(Menu *pluginMenu, const char title[], const char cmd[], int val, int minval, int maxval);
	__declspec(dllexport) MenuItem *MakeSubmenu(Menu *parentMenu, Menu *childMenu, const char title[]);
	__declspec(dllexport) Menu *MakeMenu(const char headerText[]);
	__declspec(dllexport) void DelMenu(Menu *pluginMenu);

/*	__declspec(dllexport) HFONT CreateStyleFont(StyleItem * si);
	__declspec(dllexport) void CreateBorder(HDC hdc, RECT* p_rect, COLORREF borderColour, int borderWidth);
	__declspec(dllexport) void MakeStyleGradient(hDC hDC, RECT *rect, StyleItem *si, bool withBorder);*/
	__declspec(dllexport) void MakeGradient(HDC hDC, RECT rect, int gradientType, COLORREF colorFrom, COLORREF colorTo, bool interlaced, int bevelStyle, int bevelPosition, int bevelWidth, COLORREF borderColour, int borderWidth);
/*
	__declspec(dllexport) int GetTraySize(void);
	__declspec(dllexport) SystemTray* GetTrayIcon(int pointer);

	__declspec(dllexport) char *StrRemoveEncap(char *string);*/
	__declspec(dllexport) void ParseItem(const char szItem[], StyleItem *item);
	__declspec(dllexport) bool IsInString(const char inputString[], const char searchString[]);
}

#endif // _BBRNR_H_