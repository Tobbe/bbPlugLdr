#include "bbRnr.h"
#include "MessageMap.h"
#include "UnitTests.h"
#include "BBColor.h"
#include "Utils.h"

HWND hwnd;
const char className[] = "bbRnr";
const char bbVersion[] = "bbLean 1.16 impersonator";
MessageMap msgMap;
BBColor bbc;
Utils utils;

void execCommand(const char *cmd);

#ifdef _DEBUG
UnitTests tests;
#endif // _DEBUG

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
		case WM_CREATE:
			#ifdef _DEBUG
				tests.runTests();
			#endif // _DEBUG
	
			break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	MSG msg;

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
		WS_EX_CLIENTEDGE,
		className,
		"bbRnr", //Window title
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
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

void GetBlackboxEditor(char *editor) {
	*editor = '\0';
}

char * WINAPI GetBlackboxPath(char *pszPath, int nMaxLen) {
	GetModuleFileName(NULL, pszPath, nMaxLen);
	char *end = pszPath + strlen(pszPath) - 1;
	while (end != pszPath && *end != '\\' && *end != '/') {
		end--;
	}
	*(end + 1) = '\0';
	return pszPath;
}

const char *stylePath(const char styleFileName[]) {
	return "";
}

bool BBExecute(HWND owner, const char szOperation[], const char szCommand[], const char szArgs[], const char szDirectory[], int nShowCmd, bool noErrorMsgs) {
	return false;
}

bool ReadBool(const char filePath[], const char key[], bool defaultBool) {
	return defaultBool;
}

int ReadInt(const char filePath[], const char key[], int defaultInt) {
	return defaultInt;
}

const char* const ReadString(const char filePath[], const char key[], const char defaultString[]) {
	return defaultString;
}

COLORREF ReadColor(const char filePath[], const char key[], const char defaultString[]) {
	return bbc.readColorFromString(defaultString);
}

void WriteBool(const char filePath[], const char key[], bool value) {
}

void WriteInt(const char filePath[], const char key[], int value) {
}

void WriteString(const char filePath[], const char key[], const char value[]) {
}

void WriteColor(const char filePath[], const char key[], COLORREF value) {
}

bool FileExists(const char filePath[]) {
	return utils.fileExists(filePath);
}

void SnapWindowToEdge(WINDOWPOS* windowPosition, LPARAM nDist_or_pContent, unsigned int flags) {
}

bool SetTransparency(HWND hwnd, BYTE alpha) {
	// alpha: 0-255, 255=transparency off
	return true;
}

void MakeSticky(HWND window) {
}

void RemoveSticky(HWND window) {
}

bool CheckSticky(HWND window) {
	return false;
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

void MakeGradient(HDC hDC, RECT rect, int gradientType, COLORREF colourFrom, COLORREF colourTo, bool interlaced, int bevelStyle, int bevelPosition, int bevelWidth, COLORREF borderColour, int borderWidth) {
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 255, 0));

	SelectObject(hDC, hBrush);
	Rectangle(hDC, rect.left, rect.top, rect.right, rect.bottom);
	DeleteObject(hBrush);
}

bool IsInString(const char inputString[], const char searchString[]) {
	return false;
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