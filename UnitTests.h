#ifndef _UNITTESTS_H_
#define _UNITTESTS_H_

class UnitTests {
public:
	UnitTests();
	~UnitTests();
	void runTests();
protected:
	
private:
	void testLoad();
	void testBeginEnd();
	void testSecondConstructor();
	void testMultiplePlugins();
	void testPluginVector();
	void testExportedFunctions();
	void testLoadRealPlugin();

	void testMessageMapAdd();
	void testMessageMapRemove();

	void testHexAndRgbStringToColorref(); 
	void testColorStringToColorref();
	void testColorrefToString();
};

#endif // _UNITTESTS_H_