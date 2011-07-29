#include "UnitTests.h"
#include "BBPlugin.h"
#include "MessageMap.h"
#include "BBColor.h"
#include "bbRnr.h"
#include <windows.h>
#include <vector>

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
	MessageBox(NULL, buf, TEXT("testExportedFunctions"), MB_OK);
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
	COLORREF c = RGB(255, 0, 255); // 16711935
	std::string s = "ff00ff";
	if (bbc.colorRefToString(c) != s) {
		MessageBox(NULL, TEXT("Failed to convert RGB(255, 0, 255) to \"ff00ff\""), TEXT("Error in testColorrefToString"), MB_OK);
	}

	c = RGB(100, 60, 10);
	s = "643c0a";
	if (bbc.colorRefToString(c) != s) {
		MessageBox(NULL, TEXT("Failed to convert RGB(100, 60, 10) to \"643c0a\""), TEXT("Error in testColorrefToString"), MB_OK);
	}
}

void UnitTests::runTests() {
	MessageBox(NULL, TEXT("Starting test sequence"), TEXT("Testing"), MB_OK);

	testLoad();
	testBeginEnd();
	testSecondConstructor();
	testMultiplePlugins();
	testPluginVector();
	testExportedFunctions();
	testLoadRealPlugin();

	testMessageMapAdd();
	testMessageMapRemove();

	testHexAndRgbStringToColorref();
	testColorStringToColorref();
	testColorrefToString();

	MessageBox(NULL, TEXT("Test sequence done"), TEXT("Testing"), MB_OK);
}