#ifndef _UNITTESTS_H_
#define _UNITTESTS_H_

#include <windows.h>

class UnitTests {
public:
	UnitTests();
	~UnitTests();
	void runTests(HWND hWnd);
protected:
	
private:
	void testLoad();
	void testBeginEnd();
	void testSecondConstructor();
	void testMultiplePlugins();
	void testPluginVector();
	void testExportedFunctions();

	void testMessageMapAdd();
	void testMessageMapRemove();

	void testHexAndRgbStringToColorref(); 
	void testColorStringToColorref();
	void testColorrefToString();

	void testHorizontalGradient(HWND hWnd);
	void testOtherGradients(HWND hWnd);

	void testIsInString();
	void testSettings();
	void testRCSettings();

	void testLoadRealPlugin();
};

#endif // _UNITTESTS_H_