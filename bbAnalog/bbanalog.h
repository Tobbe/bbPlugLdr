/*
 ============================================================================
 Blackbox for Windows: Plugin BBAnalog by Brad Bartolucci made using SDK example
 ============================================================================
 Copyright © 2001-2002 The Blackbox for Windows Development Team
 http://desktopian.org/bb/ - #bb4win on irc.freenode.net
 ============================================================================

  Blackbox for Windows is free software, released under the
  GNU General Public License (GPL version 2 or later), with an extension
  that allows linking of proprietary modules under a controlled interface.
  What this means is that plugins etc. are allowed to be released
  under any license the author wishes. Please note, however, that the
  original Blackbox gradient math code used in Blackbox for Windows
  is available under the BSD license.

  http://www.fsf.org/licenses/gpl.html
  http://www.fsf.org/licenses/gpl-faq.html#LinkingOverControlledInterface
  http://www.xfree86.org/3.3.6/COPYRIGHT2.html#5

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  For additional license information, please read the included license.html

  You will need the gdi+ sdk availible in the Windows core sdk at MS.

  Redistributable gdi+ runtime driver for all Windows OS's
  http://download.microsoft.com/download/platformsdk/redist/3097/W98NT42KMeXP/EN-US/gdiplus_dnld.exe

  Wav sounds can be found at http://www.a1freesoundeffects.com/radio.html

 ============================================================================
*/

#ifndef __BBANALOG_H
#define __BBANALOG_H

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

//temporary until upgrade to .9x BBAPI
#ifndef SLIT_ADD
#define SLIT_ADD 11001
#endif

#ifndef SLIT_REMOVE
#define SLIT_REMOVE 11002
#endif
//end of temp fix

#ifndef ULONG_PTR
#define ULONG_PTR DWORD
#endif

#ifndef WS_EX_LAYERED
#define WS_EX_LAYERED	0x00080000
#define LWA_COLORKEY	0x00000001
#define LWA_ALPHA		0x00000002
#endif // ndef WS_EX_LAYERED


#include <windows.h>
#include <math.h>
#include <time.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include "BBApi.h"
#include "MessageBox.h"

#pragma comment(lib, "Blackbox.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "MessageBox.lib")


//#include "StdAfx.h"

#define IDT_TIMER 1
//typedef unsigned __int64 ULONG_PTR;
//typedef DWORD ULONG_PTR;
typedef float REAL;
//typedef unsigned __int64* ULONG_PTR;

//===========================================================================

//gdi+ structs
Gdiplus::GdiplusStartupInput	gdiplusStartupInput;
ULONG_PTR						gdiplusToken;

//OS info storage
DWORD      dwId;
DWORD      dwMajorVer;
DWORD      dwMinorVer;

//for circle calculations
double	PI = 3.14159265359;
float	radius;
double	theta;
float	cntX;
float	cntY;

//the rest of these are all gdi+ objects
Gdiplus::Graphics		*graphics;
Gdiplus::Pen			*pen;
Gdiplus::FontFamily		*fontFamily;
Gdiplus::Font			*font;
Gdiplus::StringFormat	*format;
Gdiplus::Color			*plusFontColor;
Gdiplus::Color			*plusYellowColor;
Gdiplus::Color			*plusBlueColor;
Gdiplus::Color			*plusRedColor;
Gdiplus::SolidBrush		*brush;
Gdiplus::PointF			*origin;

//temp storage
static char szTemp[MAX_LINE_LENGTH];

//window instances
HINSTANCE hInstance;
HWND hwndBBAnalog, hwndBlackbox;
bool inSlit = false;	//Are we loaded in the slit? (default of no)
HWND hSlit;				//The Window Handle to the Slit (for if we are loaded)

// Blackbox messages we want to "subscribe" to:
// BB_RECONFIGURE -> Sent when changing style and on reconfigure
// BB_BROADCAST -> Broadcast message (bro@m)
int msgs[] = {BB_RECONFIGURE, BB_BROADCAST, 0};

//file path storage
char rcpath[MAX_PATH];
char stylepath[MAX_PATH];

//start up positioning
int ScreenWidth;
int ScreenHeight;

//.rc file settings
int xpos, ypos;
int width, height;
//int size;
bool wantInSlit;
bool alwaysOnTop;
bool snapWindow;
bool pluginToggle;
bool transparency;
int alpha;
bool showSeconds;
bool showCircle;
bool showRomans;
bool useRedSeconds;
bool useYellowMinutes;
bool useBlueHours;
char windowStyle[24];
char timeAlarm[24];
bool hourAlarm;
bool halfHourAlarm;
int timeAlarmFrequency;
int hourAlarmFrequency;
int halfHourAlarmFrequency;
int animationFPS;
bool synchronize;			//play wav once only if not sync'ed
//bool aubibleAlarm;		//don't need, just do .none for no wav file
char timeAlarmWav [MAX_PATH];
char hourAlarmWav [MAX_PATH];
char halfHourAlarmWav [MAX_PATH];

//time stuff
time_t systemTime;
static char currentDate[10];
struct tm *localTime;
double currentSecond, currentMinute, currentHour;

//for different drawing modes
int showDate;
int showAlarm;
char playAlarm[MAX_PATH];

int circleCounter;
bool oneMoreDraw;

bool timeAlarmSet;
//"AM" stands for the Latin phrase Ante Meridian--which means "before noon"--and
//"PM" stands for Post Meridian: "after noon." 
bool meridian;
bool magicHourFreq;
int timeAlarmHour;
int timeAlarmMinute;

//style setting storage
COLORREF backColor, backColor2;
COLORREF backColorTo, backColorTo2;
char fontFace[256];
int fontSize;
COLORREF fontColor;

bool labelIsPR;

StyleItem *myStyleItem, *myStyleItem2;
int bevelWidth;
int borderWidth;
COLORREF borderColor;

//menu items
Menu *myMenu, *windowStyleSubmenu, *configSubmenu, *settingsSubmenu;// *placementSubmenu;
Menu *clockConfigSubmenu, *alarmConfigSubmenu, *setAlarmsConfigSubmenu, *generalConfigSubmenu;

//===========================================================================

//gets OS version
int WINAPI _GetPlatformId(DWORD *pdwId, DWORD *pdwMajorVer, DWORD *pdwMinorVer);
//window process
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//special function for alpha transparency
//so there you just use BBSLWA like normal SLWA
//(c)grischka
BOOL WINAPI BBSetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);

//Plugin functions
void GetStyleSettings();
void ReadRCSettings();
void WriteRCSettings();
void InitBBAnalog();
void getCurrentTime();
//void getCurrentDate();
void mySetTimer(int mode);
void readSetAlarmTime();
void myPlayWav(char * alarmFile);

//===========================================================================

extern "C"
{
	__declspec(dllexport) int beginPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) void endPlugin(HINSTANCE hMainInstance);
	__declspec(dllexport) LPCSTR pluginInfo(int field);
	// This is the function BBSlit uses to load your plugin into the Slit
	__declspec(dllexport) int beginSlitPlugin(HINSTANCE hMainInstance, HWND hBBSlit);
}

//===========================================================================

#endif
