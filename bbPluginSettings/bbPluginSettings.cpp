#pragma comment(lib, "blackbox.lib")

#include "bbPluginSettings.h"

// Global
HINSTANCE hInstance;
HWND hWndPlugin, hWndBlackbox;
int curLength;

#define MAX_LINE_LENGTH 1024

static char rcpath[MAX_PATH];

// Configuration
static char message[128];
int showCount;
bool showOnlyOnce;

// Predefinition
//void WriteRCSettings();
//void ReadRCSettings();
void SetDefaultRCSettings();
//void ReadSettings();


void ReadRCSettings()
{
	/*char temp[MAX_LINE_LENGTH], path[MAX_LINE_LENGTH], defaultpath[MAX_LINE_LENGTH];
	int nLen;

	// First we look for the config file in the same folder as the plugin...
	GetModuleFileName(hInstance, rcpath, sizeof(rcpath));
	nLen = strlen(rcpath)-1;
	while (nLen>0 && rcpath[nLen] != '\\') nLen--;
	rcpath[nLen+1] = 0;
	strcpy(temp, rcpath);
	strcpy(path, rcpath);
	strcat(temp, "bbPluginSettings.rc");
	strcat(path, "bbPluginSettingsrc");
	// ...checking the two possible filenames bbfooman.rc and bbfoomanrc ...
	if (FileExists(temp)) {
		strcpy(rcpath, temp);
	} else if (FileExists(path)) {
		strcpy(rcpath, path);
	} else { // ...if not found, we try the Blackbox directory...
		// ...but first we save the default path (bbfooman.rc in the same
		// folder as the plugin) just in case we need it later (see below)...
		strcpy(defaultpath, temp);
		GetBlackboxPath(rcpath, sizeof(rcpath));
		strcpy(temp, rcpath);
		strcpy(path, rcpath);
		strcat(temp, "bbPluginSettings.rc");
		strcat(path, "bbPluginSettingsrc");
		if (FileExists(temp)) {
			strcpy(rcpath, temp);
		} else if (FileExists(path)) {
			strcpy(rcpath, path);
		} else { // If no config file was found, we use the default path and settings, and return
			strcpy(rcpath, defaultpath);
			SetDefaultRCSettings();
			WriteRCSettings();
			return;
		}
	}
	// If a config file was found we read the plugin settings from the file...
	// Always checking non-bool values to make sure they are the right format
	ReadSettings();*/
}


void WriteRCSettings()
{
	/*WriteString(rcpath, "bbPluginSettings.message:", message);
	WriteBool(rcpath, "bbPluginSettings.showOnlyOnce:", showOnlyOnce);
	WriteInt(rcpath, "bbPluginSettings.showCount:", showCount);*/
}

void ReadSettings()
{
	/*strcpy(message, ReadString(rcpath, "bbPluginSettings.message:", "This is a test"));
	showOnlyOnce = ReadBool(rcpath, "bbfooman.setBBITitle:", false);
	showCount = ReadInt(rcpath, "bbPluginSettings.showCount:", 2);*/
}

void SetDefaultRCSettings()
{
	strcpy_s(message, 128, "This is the default test");
	showOnlyOnce = false;
	showCount = 2;
}

extern "C" int CDECL beginPlugin(HINSTANCE hPluginInstance)
{
	WNDCLASS wc;
	hWndBlackbox = GetBBWnd();
	//hWndBlackbox = NULL;
	hInstance = hPluginInstance;
	ZeroMemory(&wc, sizeof(wc));
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hPluginInstance;
	wc.lpszClassName = szAppName;
	if(!RegisterClass(&wc)) {
		MessageBox(hWndBlackbox, "Error registering window class", szVersion, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	hWndPlugin = CreateWindow(szAppName, 0, 0, 0, 0, 0, 0, 0, 0, hPluginInstance, 0);
	if(!hWndPlugin)	{						   
		MessageBox(hWndBlackbox, "Error creating window", szVersion, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

//	ReadRCSettings();
	SetDefaultRCSettings();

	SendMessage(hWndBlackbox, BB_REGISTERMESSAGE, (WPARAM)hWndPlugin, (LPARAM)msgs);
	return 0;
}

extern "C" void CDECL endPlugin(HINSTANCE hPluginInstance)
{
	//WriteRCSettings();
	
	DestroyWindow(hWndPlugin);
	SendMessage(hWndBlackbox, BB_UNREGISTERMESSAGE, (WPARAM)hWndPlugin, (LPARAM)msgs);
	UnregisterClass(szAppName, hPluginInstance);
}

extern "C" LPCSTR CDECL pluginInfo(int field)
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
		case BB_BROADCAST:	//  bro@ms...
			return SwitchBroams((char*)lParam);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

int SwitchBroams(char *broam)
{
	char *szTemp = broam;
	if(_strnicmp(szTemp, "@BBPluginSettings.", 18)) {
		//Not for me
		return 0;
	}

	szTemp += 10; //Delete "@BBPlugin."
	// bbFooman internal commands
	if(!_stricmp(szTemp, "About")) {
		MessageBox(hWndBlackbox, "Foobar2k remote control plugin.\n"
					 "Write for using with BBInterface.\n\n"
					 "Powered by RustyNail, 2007\n"
					 "Nail.mailme [at] gmail.com",
					 szVersion, MB_OK | MB_ICONASTERISK | MB_TOPMOST);
	}
	if (!_stricmp(szTemp, "Broam")) {
		MessageBox(hWndBlackbox, "Bro@m brooooaaam bruuuuuuuum", szVersion, MB_OK);
	}

	return 0;
}
