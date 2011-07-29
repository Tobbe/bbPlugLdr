#include "../LitestepHEAD/lsapi/lsapi.h"
#include "bbPlugLdr.h"
#include "MessageMap.h"
#include "UnitTests.h"
#include "BBColor.h"
#include "Files.h"
#include "Settings.h"
#include "../xSDK/xExports.h"
#include "debug.hpp"
#include <cctype>
#include <algorithm>

HWND hwnd;
const char className[] = "bbPlugLdr";
const char version[] = "0.1";
const char bbVersion[] = "bbLean 1.16 impersonator";
MessageMap msgMap;
BBColor bbc;
Files files;
Settings settings;

void execCommand(const char *cmd);

#ifdef _DEBUG
bool firstShow = true;
UnitTests tests;
#endif // _DEBUG

int lsMessages[] = {LM_GETREVID, 0};

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case BB_REGISTERMESSAGE:
			msgMap.addMessages(reinterpret_cast<unsigned int*>(lParam), reinterpret_cast<HWND>(wParam));
			break;
		case BB_UNREGISTERMESSAGE:
			msgMap.removeMessages(reinterpret_cast<unsigned int*>(lParam), reinterpret_cast<HWND>(wParam));
			break;
		case BB_EXECUTE:
			if (lParam) {
				execCommand(reinterpret_cast<const char*>(lParam));
			}
			break;
		case LM_GETREVID: {
			std::string ver = std::string(className);
			ver += ": ";
			ver += version;
			strcpy_s(reinterpret_cast<char *>(lParam), 64, ver.c_str());
			return ver.length();
		}
		case WM_PAINT: {
			BITMAP bm;
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			#ifdef _DEBUG
				if (firstShow) {
					firstShow = false;
					tests.runTests(hwnd);
				}
			#endif // _DEBUG
			EndPaint(hwnd, &ps);
	
			break;
		}
		case WM_CLOSE:
			//DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			//PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
extern "C" int initModuleEx(HWND hwndParent, HINSTANCE hInstance, const char *path) {
	WNDCLASSEX wc;

	wc.cbSize        = sizeof(WNDCLASSEX);
	wc.style         = 0;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = className;
	wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		className,
		"bbPlugLdr", //Window title
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 140,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL) {
		UnregisterClass(className, hInstance);
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	//ShowWindow(hwnd, true);

	// Register for Litestep messages that we're interested in
	SendMessage(GetLitestepWnd(), LM_REGISTERMESSAGE, reinterpret_cast<WPARAM>(hwnd), reinterpret_cast<LPARAM>(lsMessages));

	//Only needed for "stickyness"
	//SetWindowLongPtr(hwnd, GWLP_USERDATA, magicDWord);

	return 0;
}

extern "C" void quitModule(HINSTANCE hInstance) {
	DestroyWindow(hwnd);
	UnregisterClass(className, hInstance);
}

HWND GetBBWnd() {
#ifdef _DEBUG
	MessageBox(NULL, "Calling GetBBWnd()", "DEBUG", MB_OK);
#endif
	return hwnd;
}

void execCommand(const char *cmd) {
	if (cmd[0] == '@') {
		SendMessage(hwnd, BB_BROADCAST, 0, reinterpret_cast<LPARAM>(cmd));
	} else {
		MessageBox(NULL, TEXT("BBExecute_string"), TEXT("execCommand"), MB_OK);
		//BBExecute_string(cmd, false);
		/*
		BOOL BBExecute_command(const char *command, const char *arguments, bool no_errors)
		{
		char parsed_command[MAX_PATH];
		replace_shellfolders(parsed_command, command, true);
		char workdir[MAX_PATH];
		get_directory(workdir, parsed_command);
		return BBExecute(NULL, NULL, parsed_command, arguments, workdir, SW_SHOWNORMAL, no_errors);
		}

		BOOL BBExecute_string(const char *command, bool no_errors)
		{
		char path[MAX_PATH];
		NextToken(path, &command);
		return BBExecute_command(path, command, no_errors);
		}
		*/
	}
}

const char* const GetBBVersion() {
	return bbVersion;
}

/**
 * Get the text editor the user has told Blackbox to use
 */
void GetBlackboxEditor(char *editor) {
	strcpy_s(editor, MAX_PATH, settings.getString("blackbox.editor:").c_str());
}

/**
 * Returns the path of the blackbox.exe module
 */
char * WINAPI GetBlackboxPath(char *pszPath, int nMaxLen) {
	GetModuleFileName(NULL, pszPath, nMaxLen);
	char *end = pszPath + strlen(pszPath) - 1;
	while (end != pszPath && *end != '\\' && *end != '/') {
		end--;
	}
	*(end + 1) = '\0';
	return pszPath;
}

/**
 * Returns the path of the style configuration file
 */
const char *stylePath(const char styleFileName[]) {
	return settings.getString("styleRCPath").c_str();
}

bool BBExecute(HWND owner, const char szOperation[], const char szCommand[], const char szArgs[], const char szDirectory[], int nShowCmd, bool noErrorMsgs) {
	return false;
}

/**
 * Reads the value of key[] and returns that value if found. If the value 
 * isn't found defaultBool is returned instead.
 * filePath[] is ignored, all configuration variables should be defined in a file
 * that is read by LiteStep
 */
bool ReadBool(const char filePath[], const char key[], bool defaultBool) {
	return settings.readBool(filePath, key, defaultBool);
}

/**
 * Reads the value of key[] and returns that value if found. If the value 
 * isn't found defaultInt is returned instead.
 * filePath[] is ignored, all configuration variables should be defined in a file
 * that is read by LiteStep
 */
int ReadInt(const char filePath[], const char key[], int defaultInt) {
	return settings.readInt(filePath, key, defaultInt);
}

/**
 * Reads the value of key[] and returns that value if found. If the value 
 * isn't found defaultString is returned instead.
 * filePath[] is ignored, all configuration variables should be defined in a file
 * that is read by LiteStep
 */
const char* const ReadString(const char filePath[], const char key[], const char defaultString[]) {
	const std::string &retVal = settings.readString(filePath, key, defaultString == NULL ? "" : defaultString);

	if (defaultString == NULL && retVal == "") {
		return NULL;
	}

	return retVal.c_str();
}

/**
 * Reads the value of key[] and returns that value if found. If the value 
 * isn't found defaultString is returned instead.
 * filePath[] is ignored, all configuration variables should be defined in a file
 * that is read by LiteStep
 */
COLORREF ReadColor(const char filePath[], const char key[], const char defaultString[]) {
	return settings.readColor(filePath, key, defaultString == NULL ? "" : defaultString);
}

/**
 * Writes the value of value to the key key[] in the file specified by
 * filePath[]. If the key is already defined in the file it will be removed
 * and readded to the end of the file with its new value.
 */
void WriteBool(const char filePath[], const char key[], bool value) {
	settings.writeBool(filePath, key, value);
}

/**
 * Writes the value of value to the key key[] in the file specified by
 * filePath[]. If the key is already defined in the file it will be removed
 * and readded to the end of the file with its new value.
 */
void WriteInt(const char filePath[], const char key[], int value) {
	settings.writeInt(filePath, key, value);
}

/**
 * Writes the value of value to the key key[] in the file specified by
 * filePath[]. If the key is already defined in the file it will be removed
 * and readded to the end of the file with its new value.
 */
void WriteString(const char filePath[], const char key[], const char value[]) {
	settings.writeString(filePath, key, value);
}

/**
 * Writes the value of value to the key key[] in the file specified by
 * filePath[]. If the key is already defined in the file it will be removed
 * and readded to the end of the file with its new value.
 */
void WriteColor(const char filePath[], const char key[], COLORREF value) {
	settings.writeColor(filePath, key, value);
}

/**
 * Checks if the file specified in filePath[] exists
 */
bool FileExists(const char filePath[]) {
	return files.fileExists(filePath);
}

void SnapWindowToEdge(WINDOWPOS* windowPosition, LPARAM nDist_or_pContent, unsigned int flags) {
}

bool SetTransparency(HWND hwnd, BYTE alpha) {
	// alpha: 0-255, 255=transparency off
	return true;
}

void MakeSticky(HWND window) {
	SetWindowLongPtr(window, GWLP_USERDATA, magicDWord);
}

void RemoveSticky(HWND window) {
	SetWindowLongPtr(window, GWLP_USERDATA, 0);
}

bool CheckSticky(HWND window) {
	return GetWindowLongPtr(window, GWLP_USERDATA) == magicDWord;
}

void ShowMenu(Menu *PluginMenu) {
}

Menu *MakeNamedMenu(const char headerText[], const char id[], bool fshow) {
	return NULL;
}

MenuItem *MakeMenuItem(Menu *pluginMenu, const char title[], const char cmd[], bool showIndicator) {
	return NULL;
}

MenuItem *MakeMenuNOP(Menu *pluginMenu, const char title[]) {
	return NULL;
}

MenuItem *MakeMenuItemString(Menu *pluginMenu, const char title[], const char cmd[], const char initString[]) {
	return NULL;
}

MenuItem *MakeMenuItemInt(Menu *pluginMenu, const char title[], const char cmd[], int val, int minval, int maxval) {
	return NULL;
}

MenuItem *MakeSubmenu(Menu *parentMenu, Menu *childMenu, const char title[]) {
	return NULL;
}

Menu *MakeMenu(const char headerText[]) {
	return NULL;
}

void DelMenu(Menu *pluginMenu) {
}

HFONT CreateStyleFont(StyleItem * si) {
	return NULL;
}

void CreateBorder(HDC hdc, RECT* p_rect, COLORREF borderColour, int borderWidth) {
}

void MakeStyleGradient(HDC hDC, RECT *rect, StyleItem *si, bool withBorder) {
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));

	SelectObject(hDC, hBrush);
	Rectangle(hDC, rect->left, rect->top, rect->right, rect->bottom);
	DeleteObject(hBrush);
}














static int bits_per_pixel = -1;
#define DITH_TABLE_SIZE (256+8)
#define SQF 181  // 181 * 181 * 2   = 65527
#define SQR 256  // sqrt(65527)     = 255.98
#define SQD  16
#define SQR_TAB_SIZE (SQF*SQF*2 / SQD) // ~ 4096

static unsigned char _dith_r_table[DITH_TABLE_SIZE];
static unsigned char _dith_g_table[DITH_TABLE_SIZE];
static unsigned char _dith_b_table[DITH_TABLE_SIZE];
static unsigned char add_r[16], add_g[16], add_b[16];

unsigned from_red;
unsigned from_green;
unsigned from_blue;
int diff_red;
int diff_green;
int diff_blue;
int width;
int height;
bool alternativ;
unsigned char dark_table[256];
unsigned char lite_table[256];
unsigned char *pixels;
unsigned char *xtab;
unsigned char *ytab;


static int imin(int a, int b)
{
	return a<b?a:b;
}

static int isgn(int x)
{
	return x>0 ? 1 : x<0 ? -1 : 0;
}

static void init_dither_tables(void)
{
	HDC hdc = GetDC(NULL);
	bits_per_pixel = GetDeviceCaps(hdc, BITSPIXEL);
	if (bits_per_pixel > 16 || bits_per_pixel < 8)
	{
		bits_per_pixel = 0;
	}
	else
	{
		int red_bits, green_bits, blue_bits;

		int i;
		// hardcoded for 16 bit display: red:5, green:6, blue:5
		red_bits    = 1 << (8 - 5);
		green_bits  = 1 << (8 - 6);
		blue_bits   = 1 << (8 - 5);
		for (i = 0; i < DITH_TABLE_SIZE; i++)
		{
			_dith_r_table[i] = imin(255, i & -red_bits    );
			_dith_g_table[i] = imin(255, i & -green_bits  );
			_dith_b_table[i] = imin(255, i & -blue_bits   );
		}

		static const unsigned char dither4[16] =
		{
		  7, 3, 6, 2,  // 3 1 3 1
		  1, 5, 0, 4,  // 0 2 0 2
		  6, 2, 7, 3,  // 3 1 3 1
		  0, 4, 1, 5   // 0 2 0 2
		};

		int dr = 8/red_bits, dg = 8/green_bits, db = 8/blue_bits;
		for (i = 0; i < 16; i++)
		{
		  int d = dither4[i];
		  add_r[i] = d / dr;
		  add_g[i] = d / dg;
		  add_b[i] = d / db;
		}
	}
	ReleaseDC(NULL, hdc);
}

//===========================================================================
// ColorDither for 16bit displays.

// Original comment from the authors of bb4*nix:
// "algorithm: ordered dithering... many many thanks to rasterman
//  (raster@rasterman.com) for telling me about this... portions of
//  this code is based off of his code in Imlib"

void TrueColorDither(void)
{
  unsigned char *p = pixels; int x, y;
  for (y = 0; y < height; y++) {
	int oy = 4 * (y & 3);
	for (x = 0; x < width; x++) {
	  int ox = oy + (x & 3);
	  p[0] = _dith_b_table[p[0] + add_b[ox]];
	  p[1] = _dith_g_table[p[1] + add_g[ox]];
	  p[2] = _dith_r_table[p[2] + add_r[ox]];
	  p+=4;
	}
  }
}

void table_fn(unsigned char *p, int length, bool invert)
{
	unsigned char *c = p;
	int i, e, d;
	if (invert) i = length-1, d = e = -1;
	else i = 0, d = 1, e = length;
	while (i != e)
	{
		c[0] = from_blue  + diff_blue  * i / length;
		c[1] = from_green + diff_green * i / length;
		c[2] = from_red   + diff_red   * i / length;
		c[3] = 0;
		c += 4; i += d;
	}
}

inline void diag_fn(unsigned char *c, int x, int y)
{
	unsigned char *xp = xtab + x*4;
	unsigned char *yp = ytab + y*4;
	c[0] = ((unsigned)xp[0] + (unsigned)yp[0]) >> 1;
	c[1] = ((unsigned)xp[1] + (unsigned)yp[1]) >> 1;
	c[2] = ((unsigned)xp[2] + (unsigned)yp[2]) >> 1;
	c[3] = 0;
}

void MakeGradient(HDC hDC, RECT rect, int gradientType, COLORREF colorFrom, COLORREF colorTo, bool interlaced, int bevelStyle, int bevelPosition, int bevelWidth, COLORREF borderColour, int borderWidth) {
	BBColor bbc;

	PaintTexture *texture = Create_xTextureClass();
	PaintTextureDefaults *defaults = new PaintTextureDefaults();

	defaults->m_iPaintingMode = 2; //multicolor

	switch(gradientType) {
		case B_HORIZONTAL:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
			break;
		case B_VERTICAL:
			defaults->m_iGradientType = GRADIENT_VERTICAL;
			break;
		case B_DIAGONAL:
			defaults->m_iGradientType = GRADIENT_DIAGONAL;
			break;
		case B_CROSSDIAGONAL:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
			break;
		case B_PIPECROSS:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
			break;
		case B_ELLIPTIC:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
			break;
		case B_RECTANGLE:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
			break;
		case B_PYRAMID:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
			break;
		case B_SOLID:
			defaults->m_iGradientType = GRADIENT_NONE;
			defaults->m_iPaintingMode = 1; //singlecolor
			break;
		case B_SPLITVERTICAL:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
			break;
		case B_MIRRORHORIZONTAL:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
			break;
		case B_MIRRORVERTICAL:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
			break;
		default:
			defaults->m_iGradientType = GRADIENT_HORIZONTAL;
	}

	std::string from = bbc.colorrefToString(colorFrom);
	std::string to = bbc.colorrefToString(colorTo);
	defaults->m_strColors = from.c_str();
	defaults->m_strGradientColors = to.c_str();

	TRACE("colorFrom: %X colorTo: %X strColors: %s strGraCol: %s", colorFrom, colorTo, defaults->m_strColors, defaults->m_strGradientColors);

	texture->configure(className, "", FALSE, NULL, defaults);
	texture->apply(hDC, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	delete defaults;
	Destroy_xTextureClass(texture);

	if (gradientType == B_DIAGONAL && 1 == 0) {
		//
		// BB code
		//

		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
		int type = gradientType;
		COLORREF colour_from = colorFrom;
		COLORREF colour_to = colorTo;

		pixels = NULL;

		if (width<2)  { if (width<=0) return; width = 2; }
		if (height<2) { if (height<=0) return; height = 2; }

		int byte_size = (width * height) * 4;
		int table_size = width + height;
		if (table_size < SQR) table_size = SQR;
		pixels = new unsigned char [byte_size + table_size * 4];
		xtab = pixels + byte_size;
		ytab = xtab + width * 4;

		int i;

		i = GetBValue(colour_to) - (from_blue = GetBValue(colour_from));
		diff_blue = i + isgn(i);
		i = GetGValue(colour_to) - (from_green = GetGValue(colour_from));
		diff_green = i + isgn(i);
		i = GetRValue(colour_to) - (from_red = GetRValue(colour_from));
		diff_red = i + isgn(i);

		unsigned long *s, *d, c, *e, *f;
		int x, y, z;
		unsigned char *p = pixels;
		alternativ = false;

		switch (type)
		{
			case B_DIAGONAL:
				table_fn(xtab, width, false);
				table_fn(ytab, height, true);
				y = 0;
				do {
					x = 0;
						do {
							diag_fn(p, x, y);
							p += 4;
						} while (++x < width);
				} while (++y < height);

				break;
		}

		if (-1 == bits_per_pixel) init_dither_tables();
		if (bits_per_pixel >= 8 && true) TrueColorDither();

		BITMAPINFO bv4info;
		ZeroMemory(&bv4info, sizeof(bv4info));
		bv4info.bmiHeader.biSize = sizeof(bv4info.bmiHeader);
		bv4info.bmiHeader.biWidth = width;
		bv4info.bmiHeader.biHeight = height;
		bv4info.bmiHeader.biPlanes = 1;
		bv4info.bmiHeader.biBitCount = 32;
		bv4info.bmiHeader.biCompression = BI_RGB;
		SetDIBitsToDevice(hDC, rect.left, rect.top, width, height, 0, 0, 0, height, pixels, &bv4info, DIB_RGB_COLORS);
	}
}

bool IsInString(const char inputString[], const char searchString[]) {
	std::string input(inputString);
	std::string search(searchString);

	// xoblite-flavour plugins bad version test workaround
	if (search == "bb" && input == GetBBVersion()) {
		return false;
	}

	std::transform(input.begin(), input.end(), input.begin(), (int(*)(int))std::tolower);
	std::transform(search.begin(), search.end(), search.begin(), (int(*)(int))std::tolower);

	return input.find(search, 0) != std::string::npos;
}

int GetTraySize(void) {
	return 0;
}

SystemTray* GetTrayIcon(int pointer) {
	return NULL;
}

char *StrRemoveEncap(char *string) {
	return string;
}

void ParseItem(const char szItem[], StyleItem *item) {
}