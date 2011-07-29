#include <windows.h>
#include <math.h>
#include "BBApi.h"

char *plugin_info[] =
{
	"bbSimple 1.1",
	"bbSimple",
	"1.1",
	"Tobbe Lundberg",
	"2007-12-07",
	"",
	"my_address@gmail.com",

	"@bbSimple.About"
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
