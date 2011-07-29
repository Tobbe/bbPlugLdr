/*
 ============================================================================
 This file is part of the Blackbox for Windows source code
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

 ============================================================================
*/

#ifndef __BBAPI_H_
#define __BBAPI_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#define MAX_LINE_LENGTH 4096

// Gradient types + solid
#define B_HORIZONTAL 0
#define B_VERTICAL 1
#define B_DIAGONAL 2
#define B_CROSSDIAGONAL 3
#define B_PIPECROSS 4
#define B_ELLIPTIC 5
#define B_RECTANGLE 6
#define B_PYRAMID 7
#define B_SOLID 8

// Bevels
#define BEVEL_FLAT 0
#define BEVEL_RAISED 1
#define BEVEL_SUNKEN 2

#define BEVEL1 1
#define BEVEL2 2

// Blackbox messages
#define BB_REGISTERMESSAGE		10001
#define BB_UNREGISTERMESSAGE	10002

#define BB_QUIT					10101
#define BB_RESTART				10102
#define BB_RECONFIGURE			10103
#define BB_SETSTYLE				10104

#define BB_EDITFILE				10201	// Param3: 0=CurrentStyle 1=menu.rc, 2=plugins.rc

#define BB_MENU					10301	// Param3: 0=Main menu, 1=Workspaces menu, 2=Toolbar menu
#define BB_HIDEMENU				10302
#define BB_TOGGLETRAY			10303
#define BB_SETTOOLBARLABEL		10304	// Used to set the toolbar label (returns to normal after 2 seconds)
#define BB_TOGGLEPLUGINS		10305
#define BB_SUBMENU				10306

#define BB_SHUTDOWN				10401	// Param3: 0=Shutdown, 1=Reboot, 2=Logoff, 3=Hibernate, 4=Suspend, 5=LockWorkstation
#define BB_RUN					10402

#define BB_DESKTOPINFO			10501
#define BB_LISTDESKTOPS			10502
#define BB_SWITCHTON			10503
#define BB_BRINGTOFRONT			10504
#define BB_WORKSPACE			10505	// Param3: 0=DeskLeft, 1=DeskRight, 2=AddDesktop, 3=DelDesktop, 4=DeskSwitchToN,
										//         5=GatherWindows 6=MoveWindowLeft, 7=MoveWindowRight

#define BB_ADDWINDOW			10601	// Hook messages...
#define BB_REMOVEWINDOW			10602
#define BB_ACTIVATESHELLWINDOW	10603
#define BB_ACTIVEWIN			10604
#define BB_MINMAXWIN			10605
#define BB_WINDOWSHADE			10606
#define BB_WINDOWGROWHEIGHT		10607
#define BB_WINDOWGROWWIDTH		10608
#define BB_WINDOWLOWER			10609
#define BB_REDRAW				10610
#define BB_MINIMIZE				10611

#define BB_BROADCAST			10901	// Broadcast messages (bro@m -> the bang killah! :D <vbg>)

//===========================================================================

// Unique Blackbox DWORD
// (same as in LiteStep to make apps like Winamp sticky)
const long magicDWord = 0x49474541;

//===========================================================================

// Used for populating the workspaces menu among other things
class DesktopInfo
{
public:
	char name[32]; // name of the desktop
	bool isCurrent; // if it's the current desktop
	int number; // desktop number
};

//===========================================================================

class StyleItem
{
public:
	int bevelstyle;
	int bevelposition;
	int type;
	bool parentRelative;
	bool interlaced;
};

class Menu;

//===========================================================================

extern "C" 
{
	// Exported functions
	__declspec(dllexport) LPCSTR GetBBVersion();
	__declspec(dllexport) HWND GetBBWnd();
	__declspec(dllexport) LPCSTR GetOSInfo();
	__declspec(dllexport) bool WINAPI GetBlackboxPath(LPSTR path, int maxLength);
	__declspec(dllexport) LPSTR Tokenize(LPCSTR sourceString, LPSTR targetString, LPSTR delimiter);
	__declspec(dllexport) LPCSTR bbrcPath(LPCSTR bbrcFileName=NULL);
	__declspec(dllexport) LPCSTR menuPath(LPCSTR menurcFileName=NULL);
	__declspec(dllexport) LPCSTR plugrcPath(LPCSTR pluginrcFileName=NULL);
	__declspec(dllexport) LPCSTR stylePath(LPCSTR styleFileName=NULL);
	__declspec(dllexport) FILE *FileOpen(LPCSTR fileName);
	__declspec(dllexport) bool FileClose(FILE *filePointer);
	__declspec(dllexport) int BBTokenize (LPCSTR sourceString, LPSTR* targetStrings, DWORD numTokensToParse, LPSTR remainingString);
	__declspec(dllexport) bool ReadBool(LPCSTR filePointer, LPCSTR string, bool defaultBool);
	__declspec(dllexport) int ReadInt(LPCSTR filePointer, LPCSTR string, int defaultInt);
	__declspec(dllexport) LPSTR ReadString(LPCSTR filePointer, LPCSTR string, LPSTR defaultString);
	__declspec(dllexport) COLORREF ReadColor(LPCSTR filePointer, LPCSTR string, LPCSTR defaultString);
	__declspec(dllexport) bool ReadNextCommand(FILE *filePointer, LPSTR readLine, DWORD lineLength);
	__declspec(dllexport) void Log(LPCSTR variable, LPCSTR description);
	__declspec(dllexport) HINSTANCE BBExecute(HWND Owner, LPCSTR szOperation, LPCSTR szCommand, LPCSTR szArgs, LPCSTR szDirectory, int nShowCmd, bool noErrorMsgs);
	__declspec(dllexport) void MakeGradient(HDC hdc, RECT rect, int gradientType, COLORREF colourFrom, COLORREF colourTo, bool interlaced, int bevelStyle, int bevelPosition, int bevelWidth, COLORREF borderColour, int borderWidth);
	__declspec(dllexport) bool IsInString(LPCSTR inputString, LPCSTR searchString);
	__declspec(dllexport) Menu *MakeMenu(LPSTR HeaderText);
	__declspec(dllexport) void DelMenu(Menu *PluginMenu);
	__declspec(dllexport) void MakeMenuItem(Menu *PluginMenu, LPSTR Title, LPSTR Cmd, bool ShowIndicator);
	__declspec(dllexport) void MakeSubmenu(Menu *ParentMenu, Menu *ChildMenu, LPSTR Title);
	__declspec(dllexport) void ShowMenu(Menu *PluginMenu);
	__declspec(dllexport) void MakeMenuNOP(Menu *PluginMenu, LPSTR Title);

	// Helper functions
	__declspec(dllexport) bool FileRead(FILE *filePointer, LPSTR readString);
	__declspec(dllexport) bool FileExists(LPCSTR szFileName);
	__declspec(dllexport) LPSTR StrRemoveEncap(LPSTR string);
	__declspec(dllexport) int MBoxErrorFile(LPCSTR szFile);
	__declspec(dllexport) int MBoxErrorValue(LPCSTR szValue);
	__declspec(dllexport) bool IsAppWindow(HWND hwnd);
	__declspec(dllexport) void SnapWindowToEdge(WINDOWPOS* windowPosition, int snapDistance, bool useScreenSize);
	__declspec(dllexport) void ParseItem(LPCSTR szItem, StyleItem *item);
};

//===========================================================================

#endif /* __BBAPI_H_ */
