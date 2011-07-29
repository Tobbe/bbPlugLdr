/*
 ============================================================================

  This program is free software, released under the GNU General Public License
  (GPL version 2 or later).

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  for more details.

  http://www.fsf.org/licenses/gpl.html

 ============================================================================
*/

#pragma comment(lib, "blackbox.lib")

#include "bbSimple.h"

// Global
HINSTANCE hInstance;
HWND hWndPlugin, hWndBlackbox;
int curLength;

#define MAX_LINE_LENGTH 1024

static char rcpath[MAX_PATH];

// Configuration
static char message[128];

void SetDefaultRCSettings()
{
	strcpy_s(message, 128, "bbSimple example text");
}

extern "C" int __cdecl beginPlugin(HINSTANCE hPluginInstance)
{
	WNDCLASS wc;
	hWndBlackbox = NULL;
	//hWndBlackbox = GetBBWnd();
	hInstance = hPluginInstance;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hPluginInstance;
	wc.lpszClassName = szAppName;

	if (!RegisterClass(&wc)) {
		MessageBox(hWndBlackbox, "Error registering window class", szVersion, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	hWndPlugin = CreateWindow(szAppName, 0, 0, 0, 0, 0, 0, 0, 0, hPluginInstance, 0);
	if (!hWndPlugin) {
		MessageBox(hWndBlackbox, "Error creating window", szVersion, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	SetDefaultRCSettings();
	//ReadRCSettings();

	SendMessage(hWndBlackbox, BB_REGISTERMESSAGE, (WPARAM)hWndPlugin, (LPARAM)msgs);
	return 0;
}

extern "C" void __cdecl endPlugin(HINSTANCE hPluginInstance)
{
	//WriteRCSettings();
	
	DestroyWindow(hWndPlugin);
	SendMessage(hWndBlackbox, BB_UNREGISTERMESSAGE, (WPARAM)hWndPlugin, (LPARAM)msgs);
	UnregisterClass(szAppName, hPluginInstance);
}

extern "C" LPCSTR __cdecl pluginInfo(int field)
{
	switch(field)
	{
		case PLUGIN_NAME:
			return szAppName;
		case PLUGIN_VERSION:
			return szInfoVersion;
		case PLUGIN_AUTHOR:
			return szInfoAuthor;
		case PLUGIN_RELEASE:
			return szInfoRelDate;
		case PLUGIN_LINK:
			return szInfoLink;
		case PLUGIN_EMAIL:
			return szInfoEmail;
		case PLUGIN_BROAMS:
			return szBroamList;
		default:
			return szVersion;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case BB_BROADCAST: //  bro@ms...
			return SwitchBroams((char*)lParam);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int SwitchBroams(char *broam)
{
	char *szTemp = broam;
	if (_strnicmp(szTemp, "@BBSimple.", 10)) {
		//Not for me
		return 0;
	}

	szTemp += 10; //Delete "@BBSimple."
	// bbSimple internal commands
	if (!_stricmp(szTemp, "About")) {
		MessageBox(hWndBlackbox, "bbSimple by Tobbe Lundberg", szVersion, MB_OK | MB_ICONASTERISK | MB_TOPMOST);
	}

	return 0;
}
