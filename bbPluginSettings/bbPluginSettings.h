#define CDECL __cdecl

#include <windows.h>
#include "BBApi.h"

char *plugin_info[] =
{
	"bbPluginSettings 0.1",
	"bbPluginSettings",
	"0.1",
	"Tobbe Lundberg",
	"13.12.2007",
	"",
	"some@gmail.com",

	"@bbPluginSettings.Broam"
	"@bbPluginSettings.About"
};

#define szVersion plugin_info[0]
#define szAppName plugin_info[1]
#define szInfoVersion plugin_info[2]
#define szInfoAuthor plugin_info[3]
#define szInfoRelDate plugin_info[4]
#define szInfoLink plugin_info[5]
#define szInfoEmail plugin_info[6]
#define szBroamList plugin_info[7]

int msgs[] = {BB_BROADCAST, 0};

// Predefinition
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void SetCallbacks();
int SwitchBroams(char *broam);

