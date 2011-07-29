#ifndef _PLUGIN_H_
#define _PLUGIN_H_

// Indices for pluginInfo(int index);
/*#define PLUGIN_NAME         1
#define PLUGIN_VERSION      2
#define PLUGIN_AUTHOR       3
#define PLUGIN_RELEASE      4
#define PLUGIN_RELEASEDATE  4   // xoblite
#define PLUGIN_LINK         5
#define PLUGIN_EMAIL        6
#define PLUGIN_BROAMS       7
#define PLUGIN_UPDATE_URL   8   // for Kaloth's BBPlugManager*/

#include <string>
#include <windows.h>

typedef int (*bPfp)(HINSTANCE);
typedef void (*ePfp)(HINSTANCE);
typedef int (*bSPfp)(HINSTANCE, HWND);
typedef int (*bPEfp)(HINSTANCE, HWND);
typedef const char* (*pIfp)(int);

class BBPlugin
{
private:
	std::string path;
	std::string error;
	/*std::string name;
	std::string version;
	std::string author;
	std::string release;
	std::string link;
	std::string email;
	std::string broams;
	std::string pluginUpdateUrl;*/
	bool usingSlit;
	HMODULE hPlugin;
	bPfp beginPlugin;
	ePfp endPlugin;
	bSPfp beginSlitPlugin;
	bPEfp beginPluginEx;
	pIfp pluginInfo;

	void setPath(const std::string &path);
	void setIsUsingSlit(bool usingSlit);
	int load();
	int getExportedFunctions();
public:
	BBPlugin();
	BBPlugin(const std::string &path, HWND hSlit);
	~BBPlugin();
#ifdef _DEBUG
	void test_setPath(const std::string &path);
	void test_setIsUsingSlit(bool usingSlit);
	int test_load();
	int test_getExportedFunctions();
#endif // _DEBUG
	int runBegin();
	int runEnd();
	std::string getError();
};

#endif