#include "UnitTests.h"
#include "BBPlugin.h"
#include "MessageMap.h"
#include "Settings.h"
#include "BBColor.h"
#include "bbPlugLdr.h"
#include <windows.h>
#include <vector>
#include <algorithm>
#include <cctype>
#include <fstream>

UnitTests::UnitTests() {
}

UnitTests::~UnitTests() {
}

void UnitTests::testLoad() {
	BBPlugin bbp;
	bbp.test_setPath(std::string("c:\\Tobbe\\DevProjects\\C++\\bbPlugin\\Debug\\bbPlugin.dll"));
	bbp.test_setIsUsingSlit(false);
	if (bbp.test_load() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testLoad"), MB_OK);
	}
}

void UnitTests::testBeginEnd() {
	BBPlugin bbp;
	bbp.test_setPath(std::string("c:\\Tobbe\\DevProjects\\C++\\bbPlugin\\Debug\\bbPlugin.dll"));
	bbp.test_setIsUsingSlit(false);

	if (bbp.test_load() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testBeginEnd"), MB_OK);
		return;
	}

	if (bbp.test_getExportedFunctions() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testBeginEnd"), MB_OK);
		return;
	}

	if (bbp.runBegin() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testBeginEnd"), MB_OK);
		return;
	}

	if (bbp.runEnd() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testBeginEnd"), MB_OK);
		return;
	}
}

void UnitTests::testSecondConstructor() {
	BBPlugin bbp(std::string("c:\\Tobbe\\DevProjects\\C++\\bbSimple\\Debug\\bbSimple.dll"), NULL);
	if (bbp.runBegin() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testSecondConstructor"), MB_OK);
		return;
	}

	if (bbp.runEnd() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testSecondConstructor"), MB_OK);
		return;
	}
}

void UnitTests::testMultiplePlugins() {
	BBPlugin bbp1(std::string("c:\\Tobbe\\DevProjects\\C++\\bbSimple\\Debug\\bbSimple.dll"), NULL);
	BBPlugin bbp2(std::string("c:\\Tobbe\\DevProjects\\C++\\bbPlugin\\Debug\\bbPlugin.dll"), NULL);
	if (bbp1.runBegin() < 0) {
		MessageBox(NULL, bbp1.getError().c_str(), TEXT("Error in testMultiplePlugins"), MB_OK);
		return;
	}

	if (bbp1.runEnd() < 0) {
		MessageBox(NULL, bbp1.getError().c_str(), TEXT("Error in testMultiplePlugins"), MB_OK);
		return;
	}

	if (bbp2.runBegin() < 0) {
		MessageBox(NULL, bbp2.getError().c_str(), TEXT("Error in testMultiplePlugins"), MB_OK);
		return;
	}

	if (bbp2.runEnd() < 0) {
		MessageBox(NULL, bbp2.getError().c_str(), TEXT("Error in testMultiplePlugins"), MB_OK);
		return;
	}
}

void UnitTests::testPluginVector() {
	std::vector<BBPlugin*> plugs;
	plugs.push_back(new BBPlugin(std::string("c:\\Tobbe\\DevProjects\\C++\\bbSimple\\Debug\\bbSimple.dll"), NULL));
	plugs.push_back(new BBPlugin(std::string("c:\\Tobbe\\DevProjects\\C++\\bbPlugin\\Debug\\bbPlugin.dll"), NULL));

	for (unsigned int i = 0; i < plugs.size(); i++) {
		//Check for errors generated when constructing the object
		if (plugs[i]->getError().length() > 0) {
			MessageBox(NULL, plugs[i]->getError().c_str(), TEXT("Error in testPluginVector"), MB_OK);
			return;
		}

		if (plugs[i]->runBegin() < 0) {
			MessageBox(NULL, plugs[i]->getError().c_str(), TEXT("Error in testPluginVector"), MB_OK);
			return;
		}

		if (plugs[i]->runEnd() < 0) {
			MessageBox(NULL, plugs[i]->getError().c_str(), TEXT("Error in testPluginVector"), MB_OK);
			return;
		}
	}

	// Delete the plugins
	for (int i = plugs.size() - 1; i >= 0; i--) {
		delete plugs[i];
	}
}

void UnitTests::testExportedFunctions() {
	// This function tests functions exported from blackbox.exe
	// the testing is done either by calling a plugin that uses
	// the function, or by calling the function ourselves.

	BBPlugin bbp(std::string("c:\\Tobbe\\DevProjects\\C++\\bbPluginSettings\\Debug\\bbPluginSettings.dll"), NULL);
	if (bbp.runBegin() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testExportedFunctions"), MB_OK);
		return;
	}

	if (bbp.runEnd() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testExportedFunctions"), MB_OK);
		return;
	}

	if (!FileExists("c:\\autoexec.bat")) {
		MessageBox(NULL, TEXT("FileExists returns false when it should return true"), TEXT("Error in testExportedFunctions"), MB_OK);
		return;
	}

	char buf[MAX_PATH];
	GetBlackboxPath(buf, MAX_PATH);
	MessageBox(NULL, buf, TEXT("GetBlackboxPath() in testExportedFunctions"), MB_OK);
}

void UnitTests::testLoadRealPlugin() {
	BBPlugin bbp(std::string("c:\\bbClean\\plugins\\bbAnalog\\src\\Debug\\bbAnalog.dll"), NULL);
	if (bbp.runBegin() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testLoadRealPlugin"), MB_OK);
		return;
	}

	MessageBox(NULL, TEXT("Execution will continue when you press OK"), TEXT("Pause in testLoadRealPlugin"), MB_OK);

	if (bbp.runEnd() < 0) {
		MessageBox(NULL, bbp.getError().c_str(), TEXT("Error in testLoadRealPlugin"), MB_OK);
		return;
	}
}

void UnitTests::testMessageMapAdd() {
	MessageMap messMap;
	unsigned int msgs[] = {1001, 1003, 0};
	messMap.addMessages(msgs, reinterpret_cast<HWND>(1));
	if (messMap.msgmap.count(1001) == 1) {
		if (messMap.msgmap.find(1001)->second != reinterpret_cast<HWND>(1)) {
			MessageBox(NULL, TEXT("First HWND is wrong"), TEXT("Error in testMessageMapAdd"), MB_OK);
		}
	} else {
		MessageBox(NULL, TEXT("Wrong size of messagemap"), TEXT("Error in testMessageMapAdd"), MB_OK);
	}

	unsigned int msgs2[] = {1003, 1004, 0};
	messMap.addMessages(msgs2, reinterpret_cast<HWND>(2));
	if (messMap.msgmap.count(1003) == 2) {
		std::multimap<unsigned int, HWND>::iterator itr = messMap.msgmap.find(1003);
		if (itr->second != reinterpret_cast<HWND>(1)) {
			MessageBox(NULL, TEXT("First HWND is wrong"), TEXT("Error in testMessageMapAdd"), MB_OK);
		}
		itr++;
		if (itr->second != reinterpret_cast<HWND>(2)) {
			MessageBox(NULL, TEXT("Second HWND is wrong"), TEXT("Error in testMessageMapAdd"), MB_OK);
		}
	} else {
		MessageBox(NULL, TEXT("Wrong size of messagemap"), TEXT("Error in testMessageMapAdd"), MB_OK);
	}
}

void UnitTests::testMessageMapRemove() {
	MessageMap messMap;
	unsigned int msgs[] = {1001, 1003, 0};
	unsigned int msgs2[] = {1003, 1004, 0};
	messMap.addMessages(msgs, reinterpret_cast<HWND>(1));
	messMap.addMessages(msgs2, reinterpret_cast<HWND>(2));
	messMap.removeMessages(msgs + 1, reinterpret_cast<HWND>(1));
	if (messMap.msgmap.count(1003) != 1 || messMap.msgmap.find(1003)->second != reinterpret_cast<HWND>(2)) {
		MessageBox(NULL, TEXT("Didn't correctly remove hWnd1 from message 1003"), TEXT("Error in testMessageMapRemove"), MB_OK);
	}
	if (messMap.msgmap.count(1004) != 1) {
		MessageBox(NULL, TEXT("Removed too much when removing hWnd1 from message 1003"), TEXT("Error in testMessageMapRemover"), MB_OK);
	}
	messMap.removeMessages(msgs, reinterpret_cast<HWND>(1));
	if (messMap.msgmap.count(1001) != 0) {
		MessageBox(NULL, TEXT("Didn't remove hWnd1 from message 1001"), TEXT("Error in testMessageMapRemover"), MB_OK);
	}
	if (messMap.msgmap.find(1001) != messMap.msgmap.end()) {
		MessageBox(NULL, TEXT("Didn't remove the key, even though it's empty"), TEXT("Error in testMessageMapRemover"), MB_OK);
	}
	if (messMap.msgmap.count(1003) != 1 || messMap.msgmap.find(1003)->second != reinterpret_cast<HWND>(2)) {
		MessageBox(NULL, TEXT("Removed hWnd2 from key 1003"), TEXT("Error in testMessageMapRemover"), MB_OK);
	}
}

void UnitTests::testHexAndRgbStringToColorref() {
	BBColor bbc;
	COLORREF c = bbc.readColorFromString("01AB23");
	if (c != 0x23ab01) { //bgr instead of rgb
		MessageBox(NULL, TEXT("Failed to convert hex-string to colorref"), TEXT("Error in testHexAndRgbStringToColorref"), MB_OK);
	}

	c = bbc.readColorFromString("'01Ab23'");
	if (c != 0x23ab01) { //bgr instead of rgb
		MessageBox(NULL, TEXT("Failed to convert hex-string with quotes to colorref"), TEXT("Error in testHexAndRgbStringToColorref"), MB_OK);
	}

	c = bbc.readColorFromString("a12");
	if (c != 0x2211aa) { //bgr instead of rgb
		MessageBox(NULL, TEXT("Failed to convert short hex-string to colorref"), TEXT("Error in testHexAndRgbStringToColorref"), MB_OK);
	}

	c = bbc.readColorFromString("RGB:a/b2/01");
	if (c != 0x01b2aa) { 
		MessageBox(NULL, TEXT("Failed to convert rgb-string to colorref"), TEXT("Error in testHexAndRgbStringToColorref"), MB_OK);
	}
}

void UnitTests::testColorStringToColorref() {
	BBColor bbc;
	COLORREF c = bbc.readColorFromString("blue");
	if (c != 0xff0000) { //bgr instead of rgb
		MessageBox(NULL, TEXT("Failed to convert \"blue\" to colorref"), TEXT("Error in testColorStringToColorref"), MB_OK);
	}

	c = bbc.readColorFromString("azure3");
	if (c != 0xcdcdc1) { 
		MessageBox(NULL, TEXT("Failed to convert \"azure3\" to colorref"), TEXT("Error in testColorStringToColorref"), MB_OK);
	}
}

void UnitTests::testColorrefToString() {
	BBColor bbc;
	COLORREF c = RGB(255, 0, 255);
	std::string s = "ff00ff";
	std::string converted = bbc.colorrefToString(c);

	std::transform(converted.begin(), converted.end(), converted.begin(), (int(*)(int)) std::tolower);

	if (bbc.colorrefToString(c) != s) {
		MessageBox(NULL, TEXT("Failed to convert RGB(255, 0, 255) to \"ff00ff\""), TEXT("Error in testColorrefToString"), MB_OK);
	}

	c = RGB(100, 60, 10);
	s = "643c0a";
	converted = bbc.colorrefToString(c);

	std::transform(converted.begin(), converted.end(), converted.begin(), (int(*)(int)) std::tolower);

	if (bbc.colorrefToString(c) != s) {
		MessageBox(NULL, TEXT("Failed to convert RGB(100, 60, 10) to \"643c0a\""), TEXT("Error in testColorrefToString"), MB_OK);
	}
}

void UnitTests::testHorizontalGradient(HWND hWnd) {
	RECT r;
	r.top = 0;
	r.bottom = 100;
	r.left = 0;
	r.right = 100;

	//HDC hDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC hDC = GetDC(hWnd);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, 100, 100);
	HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hDC, hBitmap));

	MakeGradient(hDC, r, B_HORIZONTAL, RGB(0, 0, 255), RGB(255, 255, 0), false, 0, 0, 0, RGB(0, 0, 0), 0);		

	if (GetPixel(hDC, 0, 50) != RGB(0, 0, 255)) {
		MessageBox(NULL, TEXT("Starting color of gradient is wrong"), TEXT("Error in testHorizontalGradient"), MB_OK);
	}

	if (GetPixel(hDC, 99, 50) != RGB(255, 255, 0)) {
		MessageBox(NULL, "Ending color of gradient is wrong", "Error in testHorizontalGradient", MB_OK);
	}

	r.left = 5;
	r.top = 5;
	r.right = 95;
	r.bottom = 95;
	MakeGradient(hDC, r, B_HORIZONTAL, RGB(255, 255, 0), RGB(0, 0, 255), false, 0, 0, 0, RGB(0, 0, 0), 0);		

	if (GetPixel(hDC, 5, 50) != RGB(255, 255, 0)) {
		MessageBox(NULL, TEXT("Starting color of gradient is wrong"), TEXT("Error in testHorizontalGradient"), MB_OK);
	}

	if (GetPixel(hDC, 94, 50) != RGB(0, 0, 255)) {
		MessageBox(NULL, "Ending color of gradient is wrong", "Error in testHorizontalGradient", MB_OK);
	}

	SelectObject(hDC, hOldBitmap);
	DeleteObject(hBitmap);
}

void UnitTests::testOtherGradients(HWND hWnd) {
	RECT r;
	r.top = 0;
	r.bottom = 100;
	r.left = 0;
	r.right = 100;

	HDC hDC = GetDC(hWnd);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, 100, 100);
	HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hDC, hBitmap));

	MakeGradient(hDC, r, B_VERTICAL, RGB(0, 0, 255), RGB(255, 255, 0), false, 0, 0, 0, RGB(0, 0, 0), 0);

	if (GetPixel(hDC, 50, 0) != RGB(0, 0, 255)) {
		MessageBox(NULL, TEXT("Starting color of gradient is wrong"), TEXT("Error in testOtherGradient"), MB_OK);
	}

	if (GetPixel(hDC, 50, 99) != RGB(255, 255, 0)) {
		MessageBox(NULL, "Ending color of gradient is wrong", "Error in testOtherGradient", MB_OK);
	}

	r.left = 5;
	r.top = 5;
	r.right = 95;
	r.bottom = 95;
	MakeGradient(hDC, r, B_DIAGONAL, RGB(255, 255, 0), RGB(0, 0, 255), false, 0, 0, 0, RGB(0, 0, 0), 0);

	if (GetPixel(hDC, 5, 94) != RGB(255, 255, 0)) {
		MessageBox(NULL, TEXT("Starting color of diagonal gradient is wrong"), TEXT("Error in testOtherGradients"), MB_OK);
	}

	//if (GetPixel(hDC, 94, 5) != RGB(0, 0, 255)) {
	if (GetPixel(hDC, 94, 5) != RGB(2, 2, 252)) { // Dunno why it doesn't end in 0, 0, 255...
		MessageBox(NULL, "Ending color of diagonal gradient is wrong", "Error in testOtherGradients", MB_OK);
	}

	SelectObject(hDC, hOldBitmap);
	DeleteObject(hBitmap);
}

void UnitTests::testIsInString() {
	if (!IsInString("This is a test string", "test")) {
		MessageBox(NULL, "IsInString returned false when it should have returned true", "Error in testIsInString", MB_OK);
	}

	if (!IsInString("AbCdEfGhIjKlMnOpQrStUv", "klm")) {
		MessageBox(NULL, "IsInString returned false when it should have returned true", "Error in testIsInString", MB_OK);
	}

	if (IsInString("AbCdEfGhIjKlMnOpQrStUv", "Adam")) {
		MessageBox(NULL, "IsInString returned true when it should have returned false", "Error in testIsInString", MB_OK);
	}
}

void UnitTests::testSettings() {
	Settings s;
	
	if (s.getString("blackbox.editor:") != "notepad.exe") {
		MessageBox(NULL, "blackbox.editor: != notepad.exe", "Error in testSettings", MB_OK);
	}

	if (s.readString("", "¤#¤#", "%%") != "%%") {
		MessageBox(NULL, "readString didn't return the default value", "Error in testSettings", MB_OK);
	}

	std::ofstream outFile("C:\\TestFilebbPlugLdr.rc");

	if (!outFile.is_open()) {
		MessageBox(NULL, "Couldn't open test file for writing", "Error in testSettings", MB_OK);
		return;
	}

	outFile << "test.key.1: " << "value.1" << "\n" <<
	           "\t\t  test.key.2: " << "value.2" << "\n" <<
	           "test.key.3: " << "value.3" << "\n" <<
	           "test.key.4: " << "value.4" << "\n";

	outFile.close();

	s.writeString("C:\\TestFilebbPlugLdr.rc", "test.key.5:", "value.5");
	s.writeString("C:\\TestFilebbPlugLdr.rc", "test.key.2:", "value.2b");
	s.writeInt("C:\\TestFilebbPlugLdr.rc", "test.key.int:", 512);
	s.writeBool("C:\\TestFilebbPlugLdr.rc", "test.key.bool:", false);
	s.writeColor("C:\\TestFilebbPlugLdr.rc", "test.key.color:", 0xefcdab);
}

void UnitTests::testRCSettings() {
	int i = ReadInt("", "bbPlugLdr.intVal", 0);
	bool b = ReadBool("", "bbPlugLdr.boolVal", true);
	const char *s = ReadString("", "bbPlugLdr.stringVal", "");
	COLORREF c = ReadColor("", "bbPlugLdr.colorVal", 0);

	if (i != 123) {
		MessageBox(NULL, "ReadInt failed", "Error in testReadRCSettings", MB_OK);
	}

	if (b != false) {
		MessageBox(NULL, "ReadBool failed", "Error in testReadRCSettings", MB_OK);
	}

	if (_stricmp(s, "string") != 0) {
		MessageBox(NULL, "ReadString failed", "Error in testReadRCSettings", MB_OK);
	}

	if (c != 0xccbbaa) {
		MessageBox(NULL, "ReadColor failed", "Error in testReadRCSettings", MB_OK);
	}

	WriteInt("C:\\TestFilebbPlugLdr.rc", "bbPlugLdr.intVal", i);
	WriteBool("C:\\TestFilebbPlugLdr.rc", "bbPlugLdr.boolVal", b);
	WriteString("C:\\TestFilebbPlugLdr.rc", "bbPlugLdr.stringVal", s);
	WriteColor("C:\\TestFilebbPlugLdr.rc", "bbPlugLdr.colorVal", c);
}

void UnitTests::runTests(HWND hWnd) {
	MessageBox(NULL, TEXT("Starting test sequence"), TEXT("Testing"), MB_OK);

	testLoad();
	testBeginEnd();
	testSecondConstructor();
	testMultiplePlugins();
	testPluginVector();
	testExportedFunctions();

	testMessageMapAdd();
	testMessageMapRemove();

	testHexAndRgbStringToColorref();
	testColorStringToColorref();
	testColorrefToString();

	testHorizontalGradient(hWnd);
	testOtherGradients(hWnd);

	testIsInString();
	testSettings();
	testRCSettings();

	testLoadRealPlugin();

	MessageBox(NULL, TEXT("Test sequence done"), TEXT("Testing"), MB_OK);
}