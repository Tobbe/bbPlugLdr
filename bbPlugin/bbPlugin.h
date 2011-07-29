#define CDECL __cdecl

#include <windows.h>
#include <math.h>
#include "BBApi.h"

char *plugin_info[] =
{
	"bbFooman 1.1",
	"bbFooman",
	"1.1",
	"RustyNail",
	"29.08.2007",
	"",
	"Nail.mailme@gmail.com",

	"@bbFooman.FooStart"
	"@bbFooman.FooExit"
	"@bbFooman.FooShow"
	"@bbFooman.FooHide"

	"@bbFooman.Play"
	"@bbFooman.PlayTrack"
	"@bbFooman.Play_Pause"
	"@bbFooman.Stop"
	"@bbFooman.Next"
	"@bbFooman.Prev"
	"@bbFooman.Random"
	"@bbFooman.Seek"

	"@bbFooman.VolUp"
	"@bbFooman.VolDown"
	"@bbFooman.Volume"
	"@bbFooman.Mute"

	"@bbFooman.Add"
	"@bbFooman.Open"
	"@bbFooman.AddDir"

	"@bbFooman.ShowPlaylist"

	"@bbFooman.Order_Default"
	"@bbFooman.Order_Shuffle"
	"@bbFooman.Order_RepeatAll"
	"@bbFooman.Order_RepeatOne"

	"@bbFooman.About"
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
void UpdateBBInterface(int uMsg, int value);
int SwitchBroams(char *broam);
int MakePlaylist();
void ShowAlbumArt(char *szPath);

