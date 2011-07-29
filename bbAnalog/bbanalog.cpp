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

 ============================================================================
*/

#include "bbanalog.h"
#include "MessageBox.h"
#include "resource.h"

LPSTR szAppName = "BBAnalog";		// The name of our window class, etc.
LPSTR szVersion = "BBAnalog v1.7";	// Used in MessageBox titlebars

LPSTR szInfoVersion = "1.7";
LPSTR szInfoAuthor = "Mortar";
LPSTR szInfoRelDate = "2003-06-12";
LPSTR szInfoLink = "http:\\\\mortar.crackmonkey.us\\\n#bb4win on irc.freenode.net";
LPSTR szInfoEmail = "mortar@crackmonkey.us";

//===========================================================================

int beginPlugin(HINSTANCE hPluginInstance)
{
	WNDCLASS wc;
	hwndBlackbox = GetBBWnd();
	hInstance = hPluginInstance;

	// Register the window class...
	ZeroMemory(&wc,sizeof(wc));
	wc.lpfnWndProc = WndProc;			// our window procedure
	wc.hInstance = hPluginInstance;		// hInstance of .dll
	wc.lpszClassName = szAppName;		// our window class name
	if (!RegisterClass(&wc)) 
	{
		MessageBox(hwndBlackbox, "Error registering window class", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	// Initialize GDI+.
	if(Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != 0)
	{
		UnregisterClass(szAppName, hPluginInstance);
		MessageBox(0, "Error starting GdiPlus.dll", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	// Get plugin and style settings...
	ReadRCSettings();
	if(!hSlit) inSlit = false;
	else inSlit = wantInSlit;
	//initialize the plugin before getting style settings
	InitBBAnalog();
	GetStyleSettings();

	// The window in this example is fixed size...
	//width = height = size;

	// Create the window...
	hwndBBAnalog = CreateWindowEx(
						WS_EX_TOOLWINDOW,								// window style
						szAppName,										// our window class name
						NULL,											// NULL -> does not show up in task manager!
						WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,	// window parameters
						xpos,											// x position
						ypos,											// y position
						width,											// window width
						height,											// window height
						NULL,											// parent window
						NULL,											// no menu
						hPluginInstance,								// hInstance of .dll
						NULL);
	if (!hwndBBAnalog)
	{
		UnregisterClass(szAppName, hPluginInstance);
		Gdiplus::GdiplusShutdown(gdiplusToken);
		MessageBox(0, "Error creating window", szAppName, MB_OK | MB_ICONERROR | MB_TOPMOST);
		return 1;
	}

	//set static gdi+ object pointers
	plusYellowColor = new Gdiplus::Color(255, 255, 0);
	plusBlueColor = new Gdiplus::Color(0, 0, 255);
	plusRedColor = new Gdiplus::Color(255, 0, 0);
	origin = new Gdiplus::PointF();
	pen = new Gdiplus::Pen(plusFontColor[0]);
	fontFamily = new Gdiplus::FontFamily(L"Times New Roman");
	font = new Gdiplus::Font(fontFamily, 8, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	format = new Gdiplus::StringFormat;
	
	//set the firstPass flag for the PAINT method
	//firstPass = true;	

	//Start the plugin timer
	if(showAlarm <= 0)
	{
		if(showSeconds) mySetTimer(0);
		else mySetTimer(1);
	}

	if(inSlit && hSlit)// Yes, so Let's let BBSlit know.
		SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndBBAnalog);
	else inSlit = false;

	//check for windows 2000 or higher before using transparency
	if(!inSlit)
	{	
		if (transparency && (dwId == VER_PLATFORM_WIN32_NT)&&(dwMajorVer > 4))
		{
			SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
			BBSetLayeredWindowAttributes(hwndBBAnalog, NULL, (unsigned char)alpha, LWA_ALPHA);
		}
	}
	//else
	//	SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW | magicDWord);
	
	// Register to receive Blackbox messages...
	SendMessage(hwndBlackbox, BB_REGISTERMESSAGE, (WPARAM)hwndBBAnalog, (LPARAM)msgs);
	// Set magicDWord to make the window sticky (same magicDWord that is used by LiteStep)...
	SetWindowLong(hwndBBAnalog, GWL_USERDATA, magicDWord);
	// Make the window AlwaysOnTop?
	if(alwaysOnTop) SetWindowPos(hwndBBAnalog, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
	// Show the window and force it to update...
	ShowWindow(hwndBBAnalog, SW_SHOW);
	InvalidateRect(hwndBBAnalog, NULL, true);

	return 0;
}

//===========================================================================
//This function is used once in beginPlugin and in @BBAnalogReloadSettings def. found in WndProc.
//Do not initialize objects here.  Deal with them in beginPlugin and endPlugin

void InitBBAnalog()
{
	//set up center points and dimensions of clock
	//check for even size(0 is starting point from edges for x, y). fix for exact center point.
	//Have to do it this way since width is not a float and any operation would trim it.  darn types.
	if((width % 2) == 0) cntX = cntY = (float)((width / 2) - .5);
	else cntX = cntY = (float)((width - 1) / 2);
	
	//UINT uResult;               // SetTimer's return value 
	
    dwId = 0;
    dwMajorVer = 0;
    dwMinorVer = 0;

	//Init display mode counters
	showDate = 0;
	showAlarm = 0;
	circleCounter = 0;
	oneMoreDraw = false;

	//getCurrentDate();
	getCurrentTime();

	//Get Platform type
	_GetPlatformId (&dwId, &dwMajorVer, &dwMinorVer);
    
	//get screen dimensions
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
}

//===========================================================================

void endPlugin(HINSTANCE hPluginInstance)
{
	if(graphics) delete graphics;
	if(pen) delete pen;
	if(font) delete font;
	if(fontFamily) delete fontFamily;
	if(format) delete format;
	if(brush) delete brush;
	if(plusFontColor) delete plusFontColor;
	if(plusYellowColor) delete plusYellowColor;
	if(plusBlueColor) delete plusBlueColor;
	if(plusRedColor) delete plusRedColor;
	if(origin) delete origin;
	
	//shutdown the gdi+ engine
	Gdiplus::GdiplusShutdown(gdiplusToken);
	// Release our timer resources
	KillTimer(hwndBBAnalog, IDT_TIMER);
	// Write the current plugin settings to the config file...
	WriteRCSettings();
	// Delete used StyleItems...
	if (myStyleItem) delete myStyleItem;
	if (myStyleItem2) delete myStyleItem2;
	// Delete the main plugin menu if it exists (PLEASE NOTE that this takes care of submenus as well!)
	if (myMenu){ DelMenu(myMenu); myMenu = NULL;}
	// Unregister Blackbox messages...
	SendMessage(hwndBlackbox, BB_UNREGISTERMESSAGE, (WPARAM)hwndBBAnalog, (LPARAM)msgs);
	if(inSlit && hSlit)
		SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndBBAnalog);
	// Destroy our window...
	DestroyWindow(hwndBBAnalog);
	// Unregister window class...
	UnregisterClass(szAppName, hPluginInstance);
}

//===========================================================================

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{		
		// Window update process...
		case WM_PAINT:
		{
			// Create buffer hdc's, bitmaps etc.
			PAINTSTRUCT ps;  RECT r;

			//get screen buffer
            HDC hdc_scrn = BeginPaint(hwnd, &ps);

			//retrieve the coordinates of the window's client area.
			GetClientRect(hwnd, &r);

			//to prevent flicker of the display, we draw to memory first,
            //then put it on screen in one single operation. This is like this:

            //first get a new 'device context'
			HDC hdc = CreateCompatibleDC(NULL);

			HBITMAP bufbmp = CreateCompatibleBitmap(hdc_scrn, r.right, r.bottom);
            SelectObject(hdc, bufbmp);
			
			if(!inSlit)
			{
				//Make background gradient
				MakeGradient(hdc, r, myStyleItem->type,
							backColor, backColorTo,
							myStyleItem->interlaced,
							myStyleItem->bevelstyle,
							myStyleItem->bevelposition,
							bevelWidth, borderColor,
							borderWidth);
			}
			
			SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, fontColor);

			//Paint second background according to toolbar.label.color: and toolbar.label.colorto:
			if(!labelIsPR || inSlit)
			{
				if(!inSlit)
				{
					r.left = r.left + (bevelWidth + borderWidth);
					r.top = r.top + (bevelWidth + borderWidth);
					r.bottom = (r.bottom - (bevelWidth + borderWidth));
					r.right = (r.right - (bevelWidth + borderWidth));
				}
						
				//draw second background according to toolbar.label.color: and toolbar.label.colorto:
				MakeGradient(hdc, r, myStyleItem2->type,
							backColor2, backColorTo2,
							myStyleItem2->interlaced,
							myStyleItem2->bevelstyle,
							myStyleItem2->bevelposition,
							bevelWidth, borderColor, 0);
			}

			if(showDate > 0)
			{
				//Create font and draw some text
				HGDIOBJ otherfont = SelectObject(hdc,
								CreateFont(fontSize,
								0, 0, 0, FW_NORMAL,
								false, false, false,
								DEFAULT_CHARSET,
								OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY,
								DEFAULT_PITCH, fontFace));

				time(&systemTime);
				localTime = localtime(&systemTime);

				strftime(currentDate, 10, "%a", localTime);
				DrawText(hdc, currentDate, strlen(currentDate), &r, DT_TOP | DT_CENTER | DT_SINGLELINE);
				//GetTextExtentPoint32(hdc, currentDate, strlen(currentDate), ?);

				strftime(currentDate, 10, "%#d", localTime);
				DrawText(hdc, currentDate, strlen(currentDate), &r, DT_VCENTER | DT_CENTER | DT_SINGLELINE);

				strftime(currentDate, 10,"%b", localTime);
				DrawText(hdc, currentDate, strlen(currentDate), &r, DT_BOTTOM | DT_CENTER | DT_SINGLELINE);
			
				//gdi:  delete object when your done
				//delete font object because I'm done with it.
				DeleteObject(SelectObject(hdc, otherfont));
				
				//Paint to the screen
				BitBlt(hdc_scrn, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

				//Count down 3 seconds so the date can be read.
				showDate--;
				//if we are done showing the date, reset the timer back to where it was.
				if(showDate <= 0)
				{
					if(showAlarm <= 0)
					{
						//reset timer
						if(showSeconds)mySetTimer(0);
						else mySetTimer(1);
					}
					showDate = 0; //just to make sure
					getCurrentTime();
					//redraw the window after all objects are deleted
					oneMoreDraw = true;
				}
			}
			else
			{
				//upgraded to gdi+ graphics lib
				graphics = new Gdiplus::Graphics(hdc);
				format->SetAlignment(Gdiplus::StringAlignmentCenter);
				graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
				graphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
				
				pen->SetWidth(1);
				pen->SetColor(plusFontColor[0]);

				if(showCircle) graphics->DrawEllipse(pen,
													(REAL)cntX - radius,
													(REAL)cntY - radius,
													(REAL)(2 * radius),
													(REAL)(2 * radius));

				//draw pulsing circle for silent alarm.  draw first so it is on bottom.
				if(showAlarm > 0)
				{
                    graphics->DrawEllipse(pen, (REAL)(cntX - circleCounter),
										(REAL)(cntY - circleCounter),
										(REAL)(2 * circleCounter),
										(REAL)(2 * circleCounter));

					if(circleCounter == 1 && synchronize && (strcmp(playAlarm, ".none") != 0))
						myPlayWav(playAlarm);
					//	PlaySound("C:\\Blackbox\\Plugins\\BBAnalog\\mysound.wav", NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
					circleCounter++;
					if(circleCounter >= radius)
					{
						circleCounter = 0;
						showAlarm--;
					}
					if(showAlarm <= 0)
					{
						circleCounter = 0;
						showAlarm = 0;
						//reset timer
						if(showSeconds) mySetTimer(0);
						else mySetTimer(1);
						//don't getCurrentTime() so the timer doesn't get reset again
						//getCurrentTime();
						//redraw the window after all objects are deleted
						oneMoreDraw = true;
					}
				}
				
				//Draw tics(1-12) on the clock face.  12, 3, 6, 9 are 2x2 pixels, the rest are 1x1 pixel.
				//REAL are floats.  Use a cast to REAL to trim the decimal point from doubles.
				//pen->SetEndCap(Gdiplus::LineCapRound);
				//pen->SetStartCap(Gdiplus::LineCapRound);
				for(theta = 0; theta < 2 * PI; theta = theta + (PI / 6))
				{
					if(theta == 0 || theta == PI / 2 || theta == PI || theta == (3 * PI) / 2)
					{
						if(showRomans) //draw roman numerals for the 12, 3, 6, 9 hands
						{
							WCHAR string[4];
							float TcntX = cntX;
							float TcntY;
							//do some finer adjustments because of different pixel sizes of each string
							if(theta == 00)			{wcscpy(string, L"III");TcntY = cntY + .5f;}
							else if(theta == PI / 2){wcscpy(string, L"XII");TcntY = cntY - 1; TcntX = TcntX + .5f;}
							else if(theta == PI)	{wcscpy(string, L"IX");	TcntY = cntY + .5f;}
							else					{wcscpy(string, L"VI");	TcntY = cntY + 2; TcntX = TcntX - .5f;}
							
							//public final void drawString( String s, PointF p, int format )
							//Also a global adjustment on cntY i.e. (TcntY - 6)
							//Gdiplus::PointF origin( (REAL)(TcntX + ((radius - 5) * cos(theta))),
							//						(REAL)((TcntY - 6) - ((radius - 5) * sin(theta))));
							origin->X = (REAL)((TcntX + 1) + ((radius - 5) * cos(theta)));
							origin->Y = (REAL)((TcntY - 6) - ((radius - 5) * sin(theta)));
							//plusFontColor->SetFromCOLORREF(fontColor);
							graphics->DrawString(string, wcslen(string), font, origin[0], format, brush);
						}
						else	//draw large tics
						{
							pen->SetWidth(2);
							graphics->DrawLine(pen,
									(REAL)(cntX + ((radius - 3) * cos(theta))),
									(REAL)(cntY - ((radius - 3) * sin(theta))),
									(REAL)(cntX + ((radius - 1) * cos(theta))),
									(REAL)(cntY - ((radius - 1) * sin(theta))));
						}
					}
					else
					{
						if(showRomans)
						{
							pen->SetWidth(2);
							graphics->DrawLine(pen,
											(REAL)(cntX + ((radius - 3) * cos(theta))),
											(REAL)(cntY - ((radius - 3) * sin(theta))),
											(REAL)(cntX + ((radius - 1) * cos(theta))),
											(REAL)(cntY - ((radius - 1) * sin(theta))));
						}
						else
						{
							pen->SetWidth(1);
							graphics->DrawLine(pen,
											(REAL)(cntX + ((radius - 2) * cos(theta))),
											(REAL)(cntY - ((radius - 2) * sin(theta))),
											(REAL)(cntX + ((radius - 1) * cos(theta))),
											(REAL)(cntY - ((radius - 1) * sin(theta))));
						}
						//below is the old gdi method for set pixel.  It only supports int plotting,
						//which means the hands will not line up everytime.  Gdi+ does not have an equivalent.
						//So draw 1 pixel long line instead.  This maynot be as nice as a point, but at least 
						//we can plot a floating point and the hands always line up.
						//SetPixel(hdc,
						//	(int)(cntX + ((radius - 1) * cos(theta))),
						//	(int)(cntY - ((radius - 1) * sin(theta))), fontColor);
					}
				}
				
				//set pen back to 2 pixels
				pen->SetWidth(2);
				//pen->SetEndCap(Gdiplus::LineCapFlat);
				//pen->SetStartCap(Gdiplus::LineCapFlat);

				//check for color
				if(useYellowMinutes) pen->SetColor(plusYellowColor[0]);
				//Draw the minute hand, always 2 less than radius
				theta = (15 - currentMinute) * (PI / 30);
				//pen->SetEndCap(LineCapTriangle);
				graphics->DrawLine(pen, (REAL)(cntX + (1.5 * cos(theta))),
					(REAL)(cntY - (1.5 * sin(theta))),
					(REAL)(cntX + ((radius - 2) * cos(theta))),
					(REAL)(cntY - ((radius - 2) * sin(theta))));

				//check for color
				if(useBlueHours)pen->SetColor(plusBlueColor[0]);
				else pen->SetColor(plusFontColor[0]);
				
				//Draw center circle.  Gives a nice pivot looking point. semi-hollow.
				graphics->DrawEllipse(pen, (REAL)(cntX - 1.5), (REAL)(cntY - 1.5), (REAL)3, (REAL)3);

				//Draw the hour hand 65% of the radius
				theta = (3 - currentHour) * (PI / 6);
				graphics->DrawLine(pen, (REAL)(cntX + (1.5 * cos(theta))),
					(REAL)(cntY - (1.5 * sin(theta))),
					(REAL)(cntX + ((radius * .65) * cos(theta))),
					(REAL)(cntY - ((radius * .65) * sin(theta))));

				//Draw the second hand, always 1 less than the radius, whose tail is 25% of the radius
				if(showSeconds)
				{
					pen->SetWidth(1);

					//check for color
					if(useRedSeconds) pen->SetColor(plusRedColor[0]);
					else pen->SetColor(plusFontColor[0]);

					//draw in a little circle so the seconds hand looks attached
					graphics->DrawEllipse(pen, (REAL)(cntX - 1), (REAL)(cntY - 1), (REAL)2, (REAL)2);
					theta = (15 - currentSecond) * (PI / 30);
					//pen->SetEndCap(LineCapFlat);
					graphics->DrawLine(pen, (REAL)(cntX - ((radius * .25) * cos(theta))),
						(REAL)(cntY + ((radius * .25) * sin(theta))),
						(REAL)(cntX + ((radius - 1) * cos(theta))),
						(REAL)(cntY - ((radius - 1) * sin(theta))));
				}
				
				//gdi:  delete objects when your done
				if(graphics) {
					delete graphics;
					graphics = NULL;
				}
				
				//Paint to the screen
				BitBlt(hdc_scrn, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
			}
			
			// Remember to delete all objects!
			//DeleteObject(SelectObject(hdc, bufbmp));//gdi: first select and delete the bmp
			//DeleteDC(hdc);         //gdi: now delete the dc

			SelectObject(hdc, bufbmp); //mortar: select just incase it is no longer in the context
			
			DeleteDC(hdc);         //gdi: first delete the dc
			DeleteObject(bufbmp);  //gdi: now the bmp is free

			//takes care of hdc_scrn
			EndPaint(hwnd, &ps);

			if(oneMoreDraw)
			{
				oneMoreDraw = false;
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
						
			return 0;
		}
		break;

		// ==========

		// If Blackbox sends a reconfigure message, load the new style settings and update the window...
		case BB_RECONFIGURE:
		{
			if(myMenu){ DelMenu(myMenu); myMenu = NULL;}
			GetStyleSettings();
			//radius = cntX - (float)(((2 * bevelWidth) + borderWidth) - 1);
			getCurrentTime();
			InvalidateRect(hwndBBAnalog, NULL, true);
		}
		break;

		// ==========

		// Broadcast messages (bro@m -> the bang killah! :D <vbg>)
		case BB_BROADCAST:
		{
			strcpy(szTemp, (LPCSTR)lParam);

			if (!_stricmp(szTemp, "@BBShowPlugins") &&  pluginToggle && !inSlit)
			{
				// Show window and force update...
				ShowWindow( hwndBBAnalog, SW_SHOW);
				getCurrentTime();
				showAlarm = 0;
				if(showSeconds) mySetTimer(0);
				else mySetTimer(1);
				InvalidateRect( hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBHidePlugins") &&  pluginToggle && !inSlit)
			{
				// Hide window...
				ShowWindow( hwndBBAnalog, SW_HIDE);
			}
			else if (!_stricmp(szTemp, "@BBAnalogAbout"))
			{
				//char temp[MAX_LINE_LENGTH];
				
				sprintf(szTemp, "%s\n\n© 2003 %s\n\n%s",
						szVersion, szInfoEmail, szInfoLink);

				CMessageBox box(hwndBBAnalog,				// hWnd
								_T(szTemp),					// Text
								_T(szAppName),				// Caption
								MB_OK | MB_SETFOREGROUND);	// type

				box.SetIcon(IDI_ICON1, hInstance);
				box.DoModal();
				
				//sprintf(szTemp, "%s\n\n© 2003 Mortar - Brad Bartolucci\n%s\n", szVersion, szInfoEmail);
				//MessageBox(hwndBBAnalog, szTemp, szAppName, MB_OK | MB_SETFOREGROUND | MB_ICONINFORMATION);
			}
			else if (!_stricmp(szTemp, "@BBAnalogTestTimeAlarm"))
			{
				//only if it is not running.  Always check showAlarm before using mySetTimer
				if(showAlarm <= 0)
				{
					if(synchronize && (StrStrI(timeAlarmWav, ".wav") != NULL))
						strcpy(playAlarm, timeAlarmWav);
					else
					{
						myPlayWav(timeAlarmWav);
						strcpy(playAlarm, ".none");
					}

					showAlarm = timeAlarmFrequency;
					mySetTimer(2);
				}
            }
			else if (!_stricmp(szTemp, "@BBAnalogTestHourAlarm"))
			{
				//only if it is not running.  Always check showAlarm before using mySetTimer
				if(showAlarm <= 0)
				{
					if(synchronize && (StrStrI(hourAlarmWav, ".wav") != NULL))
						strcpy(playAlarm, hourAlarmWav);
					else
					{
						myPlayWav(hourAlarmWav);
						strcpy(playAlarm, ".none");
					}

					if(magicHourFreq)
						showAlarm = (int)currentHour;
					else
						showAlarm = hourAlarmFrequency;
					mySetTimer(2);
				}
            }
			else if (!_stricmp(szTemp, "@BBAnalogTestHalfHourAlarm"))
			{
				//only if it is not running.  Always check showAlarm before using mySetTimer
				if(showAlarm <= 0)
				{
					if(synchronize && (StrStrI(halfHourAlarmWav, ".wav") != NULL))
						strcpy(playAlarm, halfHourAlarmWav);
					else
					{
						myPlayWav(halfHourAlarmWav);
						strcpy(playAlarm, ".none");
					}

					showAlarm = halfHourAlarmFrequency;
					mySetTimer(2);
				}
            }
			else if (!_stricmp(szTemp, "@BBAnalogPluginToggle"))
			{
				// Hide window...
				if (pluginToggle)
					 pluginToggle = false;
				else
					 pluginToggle = true;
			}
			else if (!_stricmp(szTemp, "@BBAnalogOnTop"))
			{
				// Hide window...
				if (alwaysOnTop)
					 alwaysOnTop = false;
				else
					 alwaysOnTop = true;
				if (alwaysOnTop && !inSlit) SetWindowPos(hwndBBAnalog, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
				else SetWindowPos(hwndBBAnalog, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
			}
			else if (!_stricmp(szTemp, "@BBAnalogSlit") && showAlarm == 0)
			{
				// Does user want it in the slit...
				if (wantInSlit)
					 wantInSlit = false;
				else
					 wantInSlit = true;

				inSlit = wantInSlit;
				if(wantInSlit && hSlit)
					SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndBBAnalog);
				else if(!wantInSlit && hSlit)
					SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndBBAnalog);
				else
					inSlit = false;	

				if(!inSlit)
				{
					//check for windows 2000 or higher before using transparency
					if (transparency && (dwId == VER_PLATFORM_WIN32_NT)&&(dwMajorVer > 4))
					{
						SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
						BBSetLayeredWindowAttributes(hwndBBAnalog, NULL, (unsigned char)alpha, LWA_ALPHA);
					}
					else
						SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW);

					//set window on top is alwaysontop: is true
					if ( alwaysOnTop) SetWindowPos( hwndBBAnalog, HWND_TOPMOST, xpos, ypos, width, height, SWP_NOACTIVATE);
					else SetWindowPos( hwndBBAnalog, HWND_NOTOPMOST, xpos, ypos, width, height, SWP_NOACTIVATE);
				}
				else
					if(transparency) SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW);

				GetStyleSettings();
				//update window
				getCurrentTime();
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBAnalogTransparent"))
			{
				// Set the transparent attributes to the window
				if (transparency)
					 transparency = false;
				else
					 transparency = true;
				
				//check for windows 2000 or higher before using transparency
				if(!inSlit)
				{
					if (transparency && (dwId == VER_PLATFORM_WIN32_NT)&&(dwMajorVer > 4))
					{
						SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
						BBSetLayeredWindowAttributes(hwndBBAnalog, NULL, (unsigned char)alpha, LWA_ALPHA);
					}
					else
						SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
				}
			}
			else if (!_stricmp(szTemp, "@BBAnalogSnapToEdge"))
			{
				// Set the snapWindow attributes to the window
				if (snapWindow)
					 snapWindow = false;
				else
					 snapWindow = true;
			}
			else if (!_stricmp(szTemp, "@BBAnalogSeconds"))
			{
				if(showAlarm <= 0)
				{
					// Set the showSeconds attributes to the clock
					if (showSeconds)
					{
						showSeconds = false;
						//reset timer to minutes
						mySetTimer(1);
					}
					else
					{
						showSeconds = true;
						//reset timer to seconds
						mySetTimer(0);
					}
					getCurrentTime();
					InvalidateRect(hwndBBAnalog, NULL, true);
				}
			}
			else if (!_stricmp(szTemp, "@BBAnalogRomanNumerals"))
			{
				// Set the showCircle attributes to the clock
				if (showRomans)
					 showRomans = false;
				else
					 showRomans = true;
				getCurrentTime();
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBAnalogCircle"))
			{
				// Set the showCircle attributes to the clock
				if (showCircle)
					 showCircle = false;
				else
					 showCircle = true;
				getCurrentTime();
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBAnalogRedSeconds"))
			{
				// Set the useRedSeconds attributes to the clock
				if (useRedSeconds)
					 useRedSeconds = false;
				else
					 useRedSeconds = true;
				getCurrentTime();
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBAnalogYellowMinutes"))
			{
				// Set the useYellowMinutes attributes to the clock
				if (useYellowMinutes)
					 useYellowMinutes = false;
				else
					 useYellowMinutes = true;
				getCurrentTime();
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBAnalogBlueHours"))
			{
				// Set the useRedSeconds attributes to the clock
				if (useBlueHours)
					 useBlueHours = false;
				else
					 useBlueHours = true;
				getCurrentTime();
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBAnalogHourAlarm"))
			{
				// Set the hourAlarm attributes to the clock
				if (hourAlarm)
					 hourAlarm = false;
				else
					 hourAlarm = true;
			}
			else if (!_stricmp(szTemp, "@BBAnalogHalfHourAlarm"))
			{
				// Set the halfHourAlarm attributes to the clock
				if (halfHourAlarm)
					 halfHourAlarm = false;
				else
					 halfHourAlarm = true;
			}
			else if (!_stricmp(szTemp, "@BBAnalogSynchronize"))
			{
				// Set the synchronize the animation with wav file attributes to the clock
				if (synchronize)
					 synchronize = false;
				else
					 synchronize = true;
			}
			else if (!_stricmp(szTemp, "@BBAnalogStyleLabel"))
			{
				// Set the label attributes to the window style
				strcpy(windowStyle, "label");
				GetStyleSettings();
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBAnalogStyleWindowLabel"))
			{
				// Set the windowLabel attributes to the window style
				strcpy(windowStyle, "windowlabel");
				GetStyleSettings();
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBAnalogStyleClock"))
			{
				// Set the clock attributes to the window style
				strcpy(windowStyle, "clock");
				GetStyleSettings();
				InvalidateRect(hwndBBAnalog, NULL, true);
			}
			else if (!_stricmp(szTemp, "@BBAnalogEditRC"))
			{
				BBExecute(GetDesktopWindow(), NULL, rcpath, NULL, NULL, SW_SHOWNORMAL, false);
			}
			else if (!_stricmp(szTemp, "@BBAnalogReloadSettings"))
			{
				if(showAlarm <= 0)
				{
					//remove from slit before resetting window attributes
					if(inSlit && hSlit)
						SendMessage(hSlit, SLIT_REMOVE, NULL, (LPARAM)hwndBBAnalog);

					//Re-initialize
					ReadRCSettings();
					InitBBAnalog();
					inSlit = wantInSlit;
					GetStyleSettings();
					
					//reset timer
					if(showSeconds) mySetTimer(0);
					else mySetTimer(1);

					//check for windows 2000 or higher before using transparency
					if(!inSlit)
					{
						if (transparency && (dwId == VER_PLATFORM_WIN32_NT)&&(dwMajorVer > 4))
						{
							SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED);
							BBSetLayeredWindowAttributes(hwndBBAnalog, NULL, (unsigned char)alpha, LWA_ALPHA);
						}
						else
							SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
					}
					else if(transparency) SetWindowLong(hwndBBAnalog, GWL_EXSTYLE, WS_EX_TOOLWINDOW);

					//set window on top is alwaysontop: is true
					if ( alwaysOnTop) SetWindowPos( hwndBBAnalog, HWND_TOPMOST, xpos, ypos, width, height, SWP_NOACTIVATE);
					else SetWindowPos( hwndBBAnalog, HWND_NOTOPMOST, xpos, ypos, width, height, SWP_NOACTIVATE);

					if(inSlit && hSlit)
						SendMessage(hSlit, SLIT_ADD, NULL, (LPARAM)hwndBBAnalog);
					else inSlit = false;

					//update window
					getCurrentTime();
					InvalidateRect(hwndBBAnalog, NULL, true);
				}
			}
			else if (!_stricmp(szTemp, "@BBAnalogSaveSettings"))
			{
				WriteRCSettings();
			}
		}
		break;

		// ==========

		case WM_WINDOWPOSCHANGING:
		{
			// Is SnapWindowToEdge enabled?
			if (!inSlit && snapWindow)
			{
				// Snap window to screen edges (if the last bool is false it uses the current DesktopArea)
				if(IsWindowVisible(hwnd)) SnapWindowToEdge((WINDOWPOS*)lParam, 10, true);
			}
		}
		break;

		// ==========

		// Save window position if it changes...
		case WM_WINDOWPOSCHANGED:
		{
				WINDOWPOS* windowpos = (WINDOWPOS*)lParam;
				xpos = windowpos->x;
				ypos = windowpos->y;
		}
		break;

		// ==========

		case WM_DISPLAYCHANGE:
		{
			if(!inSlit || !hSlit)
			{
				// IntelliMove(tm)... <g>
				// (c) 2003 qwilk
				//should make this a function so it can be used on startup in case resolution changed since
				//the last time blackbox was used.
				int relx, rely;
				int oldscreenwidth = ScreenWidth;
				int oldscreenheight = ScreenHeight;
				ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
				ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
				if (xpos > oldscreenwidth / 2)
				{
					relx = oldscreenwidth - xpos;
					xpos = ScreenWidth - relx;
				}
				if (ypos > oldscreenheight / 2)
				{
					rely = oldscreenheight - ypos;
					ypos = ScreenHeight - rely;
				}
				MoveWindow(hwndBBAnalog, xpos, ypos, width, height, true);
			}
		}
		break;

		// ==========
		// Allow window to move if the cntrl key is not pressed...
		case WM_NCHITTEST:
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
				return HTCLIENT;
			else
				return HTCAPTION;
		}
		break;

		case WM_NCLBUTTONDOWN:
		{
			/* Please do not allow plugins to be moved in the slit.
			 * That's not a request..  Okay, so it is.. :-P
			 * I don't want to hear about people losing their plugins
			 * because they loaded it into the slit and then moved it to
			 * the very edge of the slit window and can't get it back..
			 */
			if(!inSlit)
				return DefWindowProc(hwnd,message,wParam,lParam);
		}
		break;

		case WM_NCLBUTTONUP: {} break;
		
		// ==========
		//Show the date when the mouse is clicked and the cntrl button is press
		case WM_LBUTTONUP: 
		{
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				if(showAlarm <= 0)
				{
					showDate = 3;
					//set timer to 1 second interval for date
					if(!showSeconds) mySetTimer(0);
					InvalidateRect(hwndBBAnalog, NULL, false);
				}
			}
		}
		break;
		
		// ==========

		case WM_LBUTTONDOWN: {} break;
		
		// ==========

		case WM_NCMBUTTONUP:
		{
			if(showAlarm <= 0)
			{
				showDate = 3;
				//set timer to 1 second interval for date
				if(!showSeconds) mySetTimer(0);
				InvalidateRect(hwndBBAnalog, NULL, false);
			}
		}
		break;
		
		// ==========

		case WM_NCMBUTTONDOWN: {} break;
		
		// ==========

		// Right mouse button clicked?
		case WM_NCRBUTTONUP:
		{	
			bool tempBool = false;
			// First we delete the main plugin menu if it exists (PLEASE NOTE that this takes care of submenus as well!)
			if(myMenu){ DelMenu(myMenu); myMenu = NULL;}

			//Now we define all menus and submenus
			if(!timeAlarmSet && !hourAlarm && !halfHourAlarm)
			{
				setAlarmsConfigSubmenu = MakeMenu("No Set Alarms");
				MakeMenuNOP(setAlarmsConfigSubmenu, "No Set Alarms");
			}
			else
			{
				setAlarmsConfigSubmenu = MakeMenu("Set Alarms");
				if(timeAlarmSet)
				{
					sprintf(szTemp, "%s Alarm Set -> Test", timeAlarm);
					MakeMenuItem(setAlarmsConfigSubmenu, szTemp, "@BBAnalogTestTimeAlarm", false);
				}
				if(hourAlarm)
					MakeMenuItem(setAlarmsConfigSubmenu, "Hour Alarm Set -> Test", "@BBAnalogTestHourAlarm", false);
				if(halfHourAlarm)
					MakeMenuItem(setAlarmsConfigSubmenu, "Half Hour Alarm Set -> Test", "@BBAnalogTestHalfHourAlarm", false);
			}


			windowStyleSubmenu = MakeMenu("Window Style");
			if(StrStrI(windowStyle, "label") != NULL && strlen(windowStyle) < 6) tempBool = true;
			MakeMenuItem(windowStyleSubmenu, "toolbar.label:", "@BBAnalogStyleLabel", tempBool);
			tempBool = false;
			if(StrStrI(windowStyle, "windowlabel") != NULL) tempBool = true;
			MakeMenuItem(windowStyleSubmenu, "toolbar.windowLabel:", "@BBAnalogStyleWindowLabel", tempBool);
			tempBool = false;
			if(StrStrI(windowStyle, "clock") != NULL) tempBool = true;
			MakeMenuItem(windowStyleSubmenu, "toolbar.clock:", "@BBAnalogStyleClock", tempBool);
			
			configSubmenu = MakeMenu("Configuration");

			generalConfigSubmenu = MakeMenu("General");
			if(hSlit) MakeMenuItem(generalConfigSubmenu, "In Slit", "@BBAnalogSlit", wantInSlit);
			MakeMenuItem(generalConfigSubmenu, "Toggle with Plugins", "@BBAnalogPluginToggle", pluginToggle);
			MakeMenuItem(generalConfigSubmenu, "Always on top", "@BBAnalogOnTop", alwaysOnTop);
			if ((dwId == VER_PLATFORM_WIN32_NT)&&(dwMajorVer > 4))
				MakeMenuItem(generalConfigSubmenu, "Transparency", "@BBAnalogTransparent", transparency);
			MakeMenuItem(generalConfigSubmenu, "Snap Window To Edge", "@BBAnalogSnapToEdge", snapWindow);
			
			clockConfigSubmenu = MakeMenu("Clock");
			MakeMenuItem(clockConfigSubmenu, "Show Seconds Hand", "@BBAnalogSeconds", showSeconds);
			MakeMenuItem(clockConfigSubmenu, "Red Seconds Hand", "@BBAnalogRedSeconds", useRedSeconds);
			MakeMenuItem(clockConfigSubmenu, "Yellow Minutes Hand", "@BBAnalogYellowMinutes", useYellowMinutes);
			MakeMenuItem(clockConfigSubmenu, "Blue Hours Hand", "@BBAnalogBlueHours", useBlueHours);
			MakeMenuItem(clockConfigSubmenu, "Roman Numerals", "@BBAnalogRomanNumerals", showRomans);
			MakeMenuItem(clockConfigSubmenu, "Show Outer Circle", "@BBAnalogCircle", showCircle);
						
			alarmConfigSubmenu = MakeMenu("Alarm");
			MakeMenuItem(alarmConfigSubmenu, "Set Alarm Time", "@BBAnalogEditRC", false);
			MakeMenuItem(alarmConfigSubmenu, "Hour Alarm", "@BBAnalogHourAlarm", hourAlarm);
			MakeMenuItem(alarmConfigSubmenu, "Half Hour Alarm", "@BBAnalogHalfHourAlarm", halfHourAlarm);
			MakeMenuItem(alarmConfigSubmenu, "Synch Animation\\Wav", "@BBAnalogSynchronize", synchronize);
			
			settingsSubmenu = MakeMenu("Settings");
			MakeMenuItem(settingsSubmenu, "Edit Settings", "@BBAnalogEditRC", false);
			MakeMenuItem(settingsSubmenu, "Reload Settings", "@BBAnalogReloadSettings", false);
			MakeMenuItem(settingsSubmenu, "Save Settings", "@BBAnalogSaveSettings", false);
	
			//placementSubmenu = MakeMenu("Placement");
			//MakeMenuItem(placementSubmenu, "Top Left", "@BBAnalogTopLeft", !strcmp(BBAnalogPlacement, "TopLeft"));
			//MakeMenuItem(placementSubmenu, "Top Center", "@BBAnalogTopCenter", !strcmp(BBAnalogPlacement, "TopCenter"));
			//MakeMenuItem(placementSubmenu, "Top Right", "@BBAnalogTopRight", !strcmp(BBAnalogPlacement, "TopRight"));
			//MakeMenuItem(placementSubmenu, "Bottom Left", "@BBAnalogBottomLeft", !strcmp(BBAnalogPlacement, "BottomLeft"));
			//MakeMenuItem(placementSubmenu, "Bottom Center", "@BBAnalogBottomCenter", !strcmp(BBAnalogPlacement, "BottomCenter"));
			//MakeMenuItem(placementSubmenu, "Bottom Right", "@BBAnalogBottomRight", !strcmp(BBAnalogPlacement, "BottomRight"));
			
			//attach defined menus together
			myMenu = MakeMenu("BBAnalog");
			MakeMenuItem(myMenu, "About", "@BBAnalogAbout", false);
			MakeSubmenu(myMenu, setAlarmsConfigSubmenu, "Set Alarms");
			MakeSubmenu(myMenu, windowStyleSubmenu, "Window Style");
			MakeSubmenu(configSubmenu, generalConfigSubmenu, "General");
			MakeSubmenu(configSubmenu, clockConfigSubmenu, "Clock");
			MakeSubmenu(configSubmenu, alarmConfigSubmenu, "Alarm");
			MakeSubmenu(myMenu, configSubmenu, "Configuration");
			MakeSubmenu(myMenu, settingsSubmenu, "Settings");
			
			//MakeMenuItem(myMenu, "Resize Window", "@BBAnalogResizeWindow", false);
	
			//MakeMenuNOP(myMenu, NULL);
            			
			//MakeSubmenu(myMenu, placementSubmenu, "Placement");

			// Finally, we show the menu...
			ShowMenu(myMenu);
		}
		break;
	
		// ==========

		case WM_NCRBUTTONDOWN: {} break;

		// ==========

		case WM_NCLBUTTONDBLCLK: 
		{
			//open control panel with:  control timedate.cpl,system,0
			BBExecute(GetDesktopWindow(), NULL, "control", "timedate.cpl,system,0", NULL, SW_SHOWNORMAL, false);
		}
		break;

		// ==========

		case WM_TIMER:
		{
			switch (wParam)
			{
				case IDT_TIMER:
				{
					getCurrentTime();
					//redraw the window
					InvalidateRect(hwndBBAnalog, NULL, false);
				} break;
			}
		}
		break;

		// ==========

		default:
			return DefWindowProc(hwnd,message,wParam,lParam);
	}
	return 0;
}

//===========================================================================
//Need to clean this function up so that it is more readable, without removing functionality.

void GetStyleSettings()
{
	// Get the path to the current style file from Blackbox...
	strcpy(stylepath, stylePath());

	// ...and some additional parameters
	bevelWidth = ReadInt(stylepath, "bevelWidth:", 2);
	borderWidth = ReadInt(stylepath, "borderWidth:", 1);

	// Get the applicable color settings from the current style...
	backColor = ReadColor(stylepath, "toolbar.color:", "#000000");
	backColorTo = ReadColor(stylepath, "toolbar.colorTo:", "#FFFFFF");
	
	borderColor = ReadColor(stylepath, "borderColor:", "#000000");

	// ...gradient type, bevel etc. from toolbar:(using a StyleItem)...
	char tempstyle[MAX_LINE_LENGTH];
	strcpy(tempstyle, ReadString(stylepath, "toolbar:", "Flat Gradient Vertical"));
	if (myStyleItem) delete myStyleItem;
	myStyleItem = new StyleItem;
	ParseItem(tempstyle, myStyleItem);

	if(StrStrI(windowStyle, "label") != NULL  && strlen(windowStyle) < 6)
	{
		// ...gradient type, bevel etc. from toolbar.label:(using a StyleItem)...
		char tempstyle2[MAX_LINE_LENGTH];
		strcpy(tempstyle2, ReadString(stylepath, "toolbar.label:", "parentrelative"));
		if (!IsInString("", tempstyle2)&&!IsInString(tempstyle2, "parentrelative"))
		{
			if (myStyleItem2) delete myStyleItem2;	//if everything is found in toolbar.label: then make a new StyleItem
			myStyleItem2 = new StyleItem;			
			ParseItem(tempstyle2, myStyleItem2);
			
			if (!IsInString("", ReadString(stylepath, "toolbar.label.color:", "")))
				backColor2 = ReadColor(stylepath, "toolbar.label.color:", "#000000");
			else
    			backColor2 = ReadColor(stylepath, "toolbar.color:", "#FFFFFF");

			if (!IsInString("", ReadString(stylepath, "toolbar.label.colorTo:", "")))
				backColorTo2 = ReadColor(stylepath, "toolbar.label.colorTo:", "#000000");
			else
				backColorTo2 = ReadColor(stylepath, "toolbar.colorTo:", "#000000");
			
			fontColor = ReadColor(stylepath, "toolbar.label.textColor:", "#FFFFFF");
			if(!inSlit) radius = cntX - (float)(((2 * bevelWidth) + borderWidth));
			else radius = cntX - (float)((bevelWidth));
			labelIsPR = false;
		}
		else
		{
			if (myStyleItem2) delete myStyleItem2;	//else use the the toolbar: settings
			myStyleItem2 = new StyleItem;
			ParseItem(tempstyle, myStyleItem2);	//use original tempstyle if "parentrelative"
			backColor2 = backColor;			//have to do this if parent relative found, it seems bb4win uses
			backColorTo2 = backColorTo;		//the toolbar.color if parent relative is found for toolbar.label
			fontColor = ReadColor(stylepath, "toolbar.textColor:", "#FFFFFF");
			if(!inSlit) radius = cntX - (float)((bevelWidth + borderWidth));
			else radius = cntX - (float)(bevelWidth);
			labelIsPR = true;
		}
	}
	else if(StrStrI(windowStyle, "windowlabel") != NULL)
	{
		// ...gradient type, bevel etc. from toolbar.windowLabel:(using a StyleItem)...
		char tempstyle2[MAX_LINE_LENGTH];
		strcpy(tempstyle2, ReadString(stylepath, "toolbar.windowLabel:", "parentrelative"));
		if (!IsInString("", tempstyle2)&&!IsInString(tempstyle2, "parentrelative"))
		{
			if (myStyleItem2) delete myStyleItem2;	//if everything is found in toolbar.windowLabel: then make a new StyleItem
			myStyleItem2 = new StyleItem;			
			ParseItem(tempstyle2, myStyleItem2);
			
			if (!IsInString("", ReadString(stylepath, "toolbar.windowLabel.color:", "")))
				backColor2 = ReadColor(stylepath, "toolbar.windowLabel.color:", "#000000");
			else
    			backColor2 = ReadColor(stylepath, "toolbar.color:", "#FFFFFF");

			if (!IsInString("", ReadString(stylepath, "toolbar.windowLabel.colorTo:", "")))
				backColorTo2 = ReadColor(stylepath, "toolbar.windowLabel.colorTo:", "#000000");
			else
				backColorTo2 = ReadColor(stylepath, "toolbar.colorTo:", "#000000");
			
			fontColor = ReadColor(stylepath, "toolbar.windowLabel.textColor:", "#FFFFFF");
			if(!inSlit) radius = cntX - (float)(((2 * bevelWidth) + borderWidth));
			else radius = cntX - (float)((bevelWidth));
			labelIsPR = false;
		}
		else
		{
			if (myStyleItem2) delete myStyleItem2;	//else use the the toolbar: settings
			myStyleItem2 = new StyleItem;
			ParseItem(tempstyle, myStyleItem2);	//use original tempstyle if "parentrelative"
			backColor2 = backColor;			//have to do this if parent relative found, it seems bb4win uses
			backColorTo2 = backColorTo;		//the toolbar.color if parent relative is found for toolbar.windowLabel
			fontColor = ReadColor(stylepath, "toolbar.textColor:", "#FFFFFF");
			if(!inSlit) radius = cntX - (float)((bevelWidth + borderWidth));
			else radius = cntX - (float)(bevelWidth);
			labelIsPR = true;
		}
	}
	else
	{
		// ...gradient type, bevel etc. from toolbar.clock:(using a StyleItem)...
		char tempstyle2[MAX_LINE_LENGTH];
		strcpy(tempstyle2, ReadString(stylepath, "toolbar.clock:", "parentrelative"));
		if (!IsInString("", tempstyle2)&&!IsInString(tempstyle2, "parentrelative"))
		{
			if (myStyleItem2) delete myStyleItem2;	//if everything is found in toolbar.clock: then make a new StyleItem
			myStyleItem2 = new StyleItem;			
			ParseItem(tempstyle2, myStyleItem2);
			
			if (!IsInString("", ReadString(stylepath, "toolbar.clock.color:", "")))
				backColor2 = ReadColor(stylepath, "toolbar.clock.color:", "#000000");
			else
    			backColor2 = ReadColor(stylepath, "toolbar.color:", "#FFFFFF");

			if (!IsInString("", ReadString(stylepath, "toolbar.clock.colorTo:", "")))
				backColorTo2 = ReadColor(stylepath, "toolbar.clock.colorTo:", "#000000");
			else
				backColorTo2 = ReadColor(stylepath, "toolbar.colorTo:", "#000000");
			
			fontColor = ReadColor(stylepath, "toolbar.clock.textColor:", "#FFFFFF");
			if(!inSlit) radius = cntX - (float)(((2 * bevelWidth) + borderWidth));
			else radius = cntX - (float)((bevelWidth));
			labelIsPR = false;
		}
		else
		{
			if (myStyleItem2) delete myStyleItem2;	//else use the the toolbar: settings
			myStyleItem2 = new StyleItem;
			ParseItem(tempstyle, myStyleItem2);	//use original tempstyle if "parentrelative"
			backColor2 = backColor;			//have to do this if parent relative found, it seems bb4win uses
			backColorTo2 = backColorTo;		//the toolbar.color if parent relative is found for toolbar.clock
			fontColor = ReadColor(stylepath, "toolbar.textColor:", "#FFFFFF");
			if(!inSlit) radius = cntX - (float)((bevelWidth + borderWidth));
			else radius = cntX - (float)(bevelWidth);
			labelIsPR = true;
		}
	}
	
	//make a new gdi+ fontColor
	if(plusFontColor) delete plusFontColor;
	plusFontColor = new Gdiplus::Color(GetRValue(fontColor), GetGValue(fontColor), GetBValue(fontColor));
	if(brush) delete brush;
	brush = new Gdiplus::SolidBrush(plusFontColor[0]);
	
	// ...font settings...
	strcpy(fontFace, ReadString(stylepath, "toolbar.font:", ""));
	if (!_stricmp(fontFace, "")) strcpy(fontFace, ReadString(stylepath, "*font:", "Tahoma"));
	fontSize = (width / 3);
	//fontColor = ReadColor(stylepath, "toolbar.label.textColor:", "#FFFFFF");
}

//===========================================================================

void ReadRCSettings()
{
	char temp[MAX_LINE_LENGTH], path[MAX_LINE_LENGTH], defaultpath[MAX_LINE_LENGTH];
	int nLen;
	magicHourFreq = false;

	// First we look for the config file in the same folder as the plugin...
	GetModuleFileName(hInstance, rcpath, sizeof(rcpath));
	nLen = strlen(rcpath) - 1;
	while (nLen >0 && rcpath[nLen] != '\\') nLen--;
	rcpath[nLen + 1] = 0;
	strcpy(temp, rcpath);
	strcpy(path, rcpath);
	strcat(temp, "bbanalog.rc");
	strcat(path, "bbanalogrc");
	// ...checking the two possible filenames bbanalog.rc and bbanalogrc ...
	if (FileExists(temp)) strcpy(rcpath, temp);
	else if (FileExists(path)) strcpy(rcpath, path);
	// ...if not found, we try the Blackbox directory...
	else
	{
		// ...but first we save the default path (bbanalog.rc in the same
		// folder as the plugin) just in case we need it later (see below)...
		strcpy(defaultpath, temp);
		GetBlackboxPath(rcpath, sizeof(rcpath));
		strcpy(temp, rcpath);
		strcpy(path, rcpath);
		strcat(temp, "bbanalog.rc");
		strcat(path, "bbanalogrc");
		if (FileExists(temp)) strcpy(rcpath, temp);
		else if (FileExists(path)) strcpy(rcpath, path);
		else // If no config file was found, we use the default path and settings, and return
		{
			strcpy(rcpath, defaultpath);
			xpos = ypos = 10;
			width = height = 55;
			alpha = 160;
			wantInSlit = true;
			alwaysOnTop = true;
			snapWindow = true;
			transparency = false;
			pluginToggle = false;
			showSeconds = true;
			useRedSeconds = false;
			useYellowMinutes = false;
			useBlueHours = false;
			showRomans = false;
			showCircle = false;
			strcpy(windowStyle, "windowlabel");
			hourAlarm = true;
			halfHourAlarm = false;
			strcpy(timeAlarm, "");
			timeAlarmFrequency = 10;
			hourAlarmFrequency = 10;
			halfHourAlarmFrequency = 10;
			animationFPS = 10;
			synchronize = true;
			strcpy(timeAlarmWav, ".none");
			strcpy(hourAlarmWav, ".none");
			strcpy(halfHourAlarmWav, ".none");
			WriteRCSettings();
			return;
		}
	}
	// If a config file was found we read the plugin settings from the file...
	//Always checking non-bool values to make sure they are the right format
	xpos = ReadInt(rcpath, "bbanalog.x:", 10);
	ypos = ReadInt(rcpath, "bbanalog.y:", 10);
	if (xpos >= GetSystemMetrics(SM_CXSCREEN)) xpos = 10;
	if (ypos >= GetSystemMetrics(SM_CYSCREEN)) ypos = 10;

	width = height = ReadInt(rcpath, "bbanalog.size:", 55);
	if(width < 5 || width > 300) width = height = 55;

	alpha = ReadInt(rcpath, "bbanalog.alpha:", 160);
	if(alpha > 255) alpha = 255;
	if(ReadString(rcpath, "bbanalog.inSlit:", NULL) == NULL) wantInSlit = true;
	else wantInSlit = ReadBool(rcpath, "bbanalog.inSlit:", true);
	
	alwaysOnTop = ReadBool(rcpath, "bbanalog.alwaysOnTop:", true);
	snapWindow = ReadBool(rcpath, "bbanalog.snapWindow:", true);
	transparency = ReadBool(rcpath, "bbanalog.transparency:", false);
	pluginToggle = ReadBool(rcpath, "bbanalog.pluginToggle:", false);
	showSeconds = ReadBool(rcpath, "bbanalog.showSeconds:", true);
	useRedSeconds = ReadBool(rcpath, "bbanalog.useRedSeconds:", false);
	useYellowMinutes = ReadBool(rcpath, "bbanalog.useYellowMins:", false);
	useBlueHours = ReadBool(rcpath, "bbanalog.useBlueHours:", false);
	showRomans = ReadBool(rcpath, "bbanalog.romanNumerals:", false);
	showCircle = ReadBool(rcpath, "bbanalog.showCircle:", false);
	
	strcpy(windowStyle, ReadString(rcpath, "bbanalog.windowStyle:", "windowlabel"));
	if(((StrStrI(windowStyle, "label") == NULL) || ((StrStrI(windowStyle, "label") != NULL) && (strlen(windowStyle) > 5))) 
		&& (StrStrI(windowStyle, "windowlabel") == NULL) && (StrStrI(windowStyle, "clock") == NULL))
		strcpy(windowStyle, "windowLabel");

	strcpy(timeAlarm, ReadString(rcpath, "bbanalog.timeAlarm:", ""));
	readSetAlarmTime();

	hourAlarm = ReadBool(rcpath, "bbanalog.hourAlarm:", true);
	halfHourAlarm = ReadBool(rcpath, "bbanalog.halfHourAlarm:", false);

	timeAlarmFrequency = ReadInt(rcpath, "bbanalog.timeAlarmFreq:", 10);
	if(timeAlarmFrequency < 1 || timeAlarmFrequency > 30) timeAlarmFrequency = 10;

	if(StrStrI(ReadString(rcpath, "bbanalog.hourAlarmFreq:", "10"), "hour") != NULL)
		magicHourFreq = true;
	else
	{
		magicHourFreq = false;
		hourAlarmFrequency = ReadInt(rcpath, "bbanalog.hourAlarmFreq:", 10);
		if(hourAlarmFrequency < 1 || hourAlarmFrequency > 30) hourAlarmFrequency = 10;
	}

	halfHourAlarmFrequency = ReadInt(rcpath, "bbanalog.halfHourAlarmFreq:", 10);
	if(halfHourAlarmFrequency < 1 || halfHourAlarmFrequency > 30) halfHourAlarmFrequency = 10;

	animationFPS = ReadInt(rcpath, "bbanalog.animationFPS:", 10);
	if(animationFPS < 1 || animationFPS > 30) animationFPS = 10;

	synchronize = ReadBool(rcpath, "bbanalog.synchronize:", true);
	strcpy(timeAlarmWav, ReadString(rcpath, "bbanalog.timeAlarmWav:", ".none"));
	strcpy(hourAlarmWav, ReadString(rcpath, "bbanalog.hourAlarmWav:", ".none"));
	strcpy(halfHourAlarmWav, ReadString(rcpath, "bbanalog.halfHourAlarmWav:", ".none"));
}

//===========================================================================

void WriteRCSettings()
{
	static char szTemp[MAX_LINE_LENGTH];
	static char temp[8];
	DWORD retLength = 0;

	// Write plugin settings to config file, using path found in ReadRCSettings()...
	HANDLE file = CreateFile(rcpath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (file)
	{
		sprintf(szTemp, "bbanalog.x: %d\r\n", xpos, temp);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		
		sprintf(szTemp, "bbanalog.y: %d\r\n", ypos, temp);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbanalog.size: %d\r\n", width, temp);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		
		sprintf(szTemp, "bbanalog.alpha: %d\r\n", alpha, temp);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(wantInSlit) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.inSlit: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(alwaysOnTop) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.alwaysOnTop: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(snapWindow) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.snapWindow: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(transparency) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.transparency: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(pluginToggle) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.pluginToggle: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(showSeconds) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.showSeconds: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		
		(useRedSeconds) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.useRedSeconds: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(useYellowMinutes) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.useYellowMins: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(useBlueHours) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.useBlueHours: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(showRomans) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.romanNumerals: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		
		(showCircle) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.showCircle: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbanalog.windowStyle: %s\r\n", windowStyle);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbanalog.timeAlarm: %s\r\n", timeAlarm);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		
		(hourAlarm) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.hourAlarm: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(halfHourAlarm) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.halfHourAlarm: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbanalog.timeAlarmFreq: %d\r\n", timeAlarmFrequency, temp);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		if(magicHourFreq)
		{
			sprintf(szTemp, "bbanalog.hourAlarmFreq: hour\r\n");
			WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		}
		else
		{
			sprintf(szTemp, "bbanalog.hourAlarmFreq: %d\r\n", hourAlarmFrequency, temp);
			WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
		}

		sprintf(szTemp, "bbanalog.halfHourAlarmFreq: %d\r\n", halfHourAlarmFrequency, temp);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbanalog.animationFPS: %d\r\n", animationFPS, temp);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		(synchronize) ? strcpy(temp, "true") : strcpy(temp, "false");
		sprintf(szTemp, "bbanalog.synchronize: %s\r\n", temp);
 		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbanalog.timeAlarmWav: %s\r\n", timeAlarmWav);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbanalog.hourAlarmWav: %s\r\n", hourAlarmWav);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);

		sprintf(szTemp, "bbanalog.halfHourAlarmWav: %s\r\n", halfHourAlarmWav);
		WriteFile(file, szTemp, strlen(szTemp), &retLength, NULL);
	}
	CloseHandle(file);
}

//===========================================================================

//Plugin info for later BB4win support
LPCSTR pluginInfo(int field)
{
	// pluginInfo is used by Blackbox for Windows to fetch information about
	// a particular plugin. At the moment this information is simply displayed
	// in an "About loaded plugins" MessageBox, but later on this will be
	// expanded into a more advanced plugin handling system. Stay tuned! :)

	switch (field)
	{
		case 1:
			return szAppName; // Plugin name
		case 2:
			return szInfoVersion; // Plugin version
		case 3:
			return szInfoAuthor; // Author
		case 4:
			return szInfoRelDate; // Release date, preferably in yyyy-mm-dd format
		case 5:
			return szInfoLink; // Link to author's website
		case 6:
			return szInfoEmail; // Author's email

		// ==========

		default:
			return szVersion; // Fallback: Plugin name + version, e.g. "MyPlugin 1.0"
	}
}

//===========================================================================

//so there you just use BBSLWA like normal SLWA
//(c)grischka
BOOL WINAPI BBSetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
	static BOOL (WINAPI *pSLWA)(HWND, COLORREF, BYTE, DWORD);
	static unsigned int f=0;
	for (;;) {
		if (2==f)   return pSLWA(hwnd, crKey, bAlpha, dwFlags);
		// if it's not there, just do nothing and report success
		if (f)      return TRUE;
		*(FARPROC*)&pSLWA = GetProcAddress(GetModuleHandle("USER32"), "SetLayeredWindowAttributes");
		f = pSLWA ? 2 : 1;
	}
}

//===========================================================================

//check for OS version
int WINAPI _GetPlatformId(DWORD *pdwId, DWORD *pdwMajorVer, DWORD *pdwMinorVer)
{
	OSVERSIONINFO  osvinfo;
	ZeroMemory(&osvinfo, sizeof(osvinfo));
	osvinfo.dwOSVersionInfoSize = sizeof (osvinfo);
	GetVersionEx(&osvinfo);
	*pdwId       = osvinfo.dwPlatformId;
	*pdwMajorVer = osvinfo.dwMajorVersion;
	*pdwMinorVer = osvinfo.dwMinorVersion;
	return 0;
}

//===========================================================================

//get current local time of the users machine
void getCurrentTime()
{
	time(&systemTime);
	localTime		= localtime(&systemTime);
	currentSecond	= localTime->tm_sec;
	currentMinute	= localTime->tm_min;
	currentHour		= localTime->tm_hour;
		
	//fix 24 hour time
	if(currentHour > 12) currentHour -= 12;
	if(currentHour == 0) currentHour = 12;

	//timeAlarm takes precedence, then hourAlarm, then halfHourAlarm

	//alarmTime alarm
	if(timeAlarmSet && (showAlarm <= 0))
	{
		//must check if alarm is pm and use 24 hour time -- midnight == 0, noon == 12
		if((meridian && (localTime->tm_hour > 11)) || (!meridian && (localTime->tm_hour < 12)))
		{
			//Set timer and counter for silent alarm, on time alarm.
			if((((currentSecond == 0) || (showSeconds != true)) && ((int)currentMinute == timeAlarmMinute) && (((int)currentHour) == timeAlarmHour)))
			{			
				if(synchronize && (StrStrI(timeAlarmWav, ".wav") != NULL))
					strcpy(playAlarm, timeAlarmWav);
				else
				{
					myPlayWav(timeAlarmWav);
					strcpy(playAlarm, ".none");
				}
				//set timer to alarm -- mode = 2
				mySetTimer(2);
				showAlarm = timeAlarmFrequency;
			}
		}
	}

	//hour alarm
	if(hourAlarm && (showAlarm <= 0))
	{
		//Set timer and counter for silent alarm, every new hour.
		if(((currentSecond == 0) && (currentMinute == 0)) || ((showSeconds != true) && (currentMinute == 0)))
		{			
			if(synchronize && (StrStrI(hourAlarmWav, ".wav") != NULL))
				strcpy(playAlarm, hourAlarmWav);
			else
			{
				myPlayWav(hourAlarmWav);
				strcpy(playAlarm, ".none");
			}
			//set timer to alarm -- mode = 2
			mySetTimer(2);
			if(magicHourFreq)
				showAlarm = (int)currentHour;
			else
				showAlarm = hourAlarmFrequency;
		}
	}

	//half hour alarm
	if(halfHourAlarm && (showAlarm <= 0))
	{
		//Set timer and counter for silent alarm, every new hour.
		if(((currentSecond == 0) && (currentMinute == 30)) || ((showSeconds != true) && (currentMinute == 30)))
		{			
			if(synchronize && (StrStrI(halfHourAlarmWav, ".wav") != NULL))
				strcpy(playAlarm, halfHourAlarmWav);
			else
			{
				myPlayWav(halfHourAlarmWav);
				strcpy(playAlarm, ".none");
			}
			//set timer to alarm -- mode = 2
			mySetTimer(2);
			showAlarm = halfHourAlarmFrequency;
		}
	}

	//for getting fraction of an hour
	currentHour += (currentMinute / 60);
}

//===========================================================================
/*
void getCurrentDate()
{
	time(&systemTime);
	localTime = localtime(&systemTime);
	strftime(currentDate, 128,"%a %#d\n%b", localTime);
}
*/

//===========================================================================
//mode == 0 -> 1 sec, mode == 1 -> 1 min, mode == 2 -> 1/fps sec
//could add a check to make sure SetTimer succeeded and return a boolean for beginPlugin :)

void mySetTimer(int mode)
{
	//1 minute timer
	if(mode == 1)
	{
		//Start the 1 minute plugin timer
		SetTimer(hwndBBAnalog,		// handle to main window 
				IDT_TIMER,			// timer identifier 
				60000,				// 1-minute interval 
				(TIMERPROC) NULL);	// no timer callback 
	}
	//alarm mode timer, for faster redraw
	else if(mode == 2)
	{
		//Start the 1/20 second plugin timer
		SetTimer(hwndBBAnalog,			// handle to main window 
				IDT_TIMER,				// timer identifier 
				(UINT)1000/animationFPS,// 1/fps-second interval - default 10 fps
				(TIMERPROC) NULL);		// no timer callback 
	}
	else	//always fall back to the 1 second timer -- mode == 0
	{
		//Start the 1 second plugin timer
		SetTimer(hwndBBAnalog,		// handle to main window 
				IDT_TIMER,			// timer identifier 
				1000,				// 1-second interval 
				(TIMERPROC) NULL);	// no timer callback 
	}
}

//===========================================================================
//parse the setTime string.  make input string empty if it is wrong and alarmTimeSet = false

void readSetAlarmTime()
{
	//bool tempBool = false;
	strcpy(szTemp, timeAlarm);
	char* pch;
	//get first token
	pch = strtok(szTemp, ":");
	if(pch != NULL)
	{
		if(strlen(pch) > 2 || strlen(pch) < 1) strcpy(timeAlarm, "");
		else
		{
			if(strlen(pch) == 2) timeAlarmHour = (((int)pch[0] - 48) * 10) + ((int)pch[1] - 48);
			else timeAlarmHour = (int)pch[0] - 48;

			//get second token
			pch = strtok (NULL, "");
			if(pch != NULL)
			{
				if((pch[1] - 48 >= 0) && (pch[1] - 48 <= 9))
				{
					timeAlarmMinute = (((int)pch[0] - 48) * 10) + ((int)pch[1] - 48);
					if(StrStrI(pch , "pm") != NULL)
						meridian = true;
					else if(StrStrI(pch , "am")  != NULL)
						meridian = false;
					else
					{
						meridian =false;
						strcpy(timeAlarm, "");
					}
				}
				else
					strcpy(timeAlarm, "");
			}
			else
				strcpy(timeAlarm, "");
		}
	}
	else
	{
		strcpy(timeAlarm, "");
	}
	
	//make sure they are of a 12 hour format with meridian designation
	if(timeAlarmHour > 12 || timeAlarmHour < 1 || timeAlarmMinute < 0 || timeAlarmMinute > 59 || strcmp(timeAlarm, "") == 0)
	{
		timeAlarmSet = false;
		strcpy(timeAlarm, "");
	}
	else
	{
		timeAlarmSet = true;
	}
}

//===========================================================================
//play wav file internally or execute the unknown file and let windows decide

void myPlayWav(char * alarmFile)
{
	int nLen;
	if(StrStrI(alarmFile, ".none") == NULL)
	{
		//First we check if the string points directly to the file
		if(FileExists(alarmFile))
		{
			if (StrStrI(alarmFile, ".wav") != NULL)
				PlaySound(alarmFile, NULL, SND_FILENAME | SND_ASYNC);
			else
				BBExecute(GetDesktopWindow(), NULL, alarmFile, NULL, NULL, SW_SHOWNORMAL, false);
		}
		// Then we look for the alarm file in the same folder as the plugin...
		else
		{
			GetModuleFileName(hInstance, szTemp, sizeof(szTemp));
			nLen = strlen(szTemp) - 1;
			while (nLen >0 && szTemp[nLen] != '\\') nLen--;
			szTemp[nLen + 1] = 0;
			strcat(szTemp, alarmFile);
			if (FileExists(szTemp))
			{
				if (StrStrI(alarmFile, ".wav") != NULL)
					PlaySound(szTemp, NULL, SND_FILENAME | SND_ASYNC);
				else
					BBExecute(GetDesktopWindow(), NULL, szTemp, NULL, NULL, SW_SHOWNORMAL, false);
			}
		}
	}
}

//===========================================================================

int beginSlitPlugin(HINSTANCE hMainInstance, HWND hBBSlit)
{
	/* Since we were loaded in the slit we need to remember the Slit
	 * HWND and make sure we remember that we are in the slit ;)
	 */
	inSlit = true;
	hSlit = hBBSlit;

	// Start the plugin like normal now..
	return beginPlugin(hMainInstance);
}