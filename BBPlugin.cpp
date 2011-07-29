#include "bbPlugin.h"
#include "Utils.h"
#include <windows.h>

BBPlugin::BBPlugin() : hPlugin(NULL), beginPlugin(NULL), endPlugin(NULL), beginSlitPlugin(NULL), beginPluginEx(NULL) {
/*BBPlugin::BBPlugin() {
	hPlugin = NULL;
	beginPlugin = NULL;
	endPlugin = NULL;
	beginSlitPlugin = NULL;
	beginPluginEx = NULL;*/
	setIsUsingSlit(false);
}

BBPlugin::BBPlugin(const std::string &path, HWND hSlit) : hPlugin(NULL) {
	setPath(path);
	setIsUsingSlit(hSlit != NULL);
	if (load() == 0) {
		getExportedFunctions();
	}
}

void BBPlugin::setPath(const std::string &path) {
	this->path = std::string(path);
}

void BBPlugin::setIsUsingSlit(bool usingSlit) {
	this->usingSlit = usingSlit;
}

int BBPlugin::load() {
	//---------------------------------------
	// load the dll

	if (hPlugin != NULL) {
		error = path + " is already loaded";
		return -1;
	}

	hPlugin = LoadLibrary(path.c_str());

	if (hPlugin == NULL) {
		int err = GetLastError();
		//dbg_printf("error %d", GetLastError());
		if (err == ERROR_DLL_NOT_FOUND || err == ERROR_MOD_NOT_FOUND) {
			Utils u;
			//char buffer[MAX_PATH];
			//string path = make_bb_path(buffer, q->fullpath);
			if (u.fileExists(path)) {
				//error = error_dll_needs_module;
				error = path + " is not a valid module";
			} else {
				error = path + " does not exist";
			}
		} else if (err == ERROR_BAD_EXE_FORMAT) {
			error = path + " is not a valid Win32 Application";
		} else {
			error = path + " is not a valid module";
		}

		return -1;
	}

	////---------------------------------------
	//// grab interface functions
	//{
	//	static const char* function_names[] = {
	//		"pluginInfo",
	//		"beginPlugin",
	//		"endPlugin",
	//		"beginSlitPlugin",
	//		"beginPluginEx"
	//	};

	//	FARPROC* pp = (FARPROC*)&q->pluginInfo;
	//	for (int i = 0; i < 5; i++) {
	//		*pp++ = GetProcAddress(hPlugin, function_names[i]);
	//	}
	//}

	////---------------------------------------
	//// check interface presence

	//if (NULL == q->endPlugin)
	//{
	//	error = error_missing_entry;
	//	break;
	//}

	//if (q->useslit && NULL == q->beginSlitPlugin && NULL == q->beginPluginEx)
	//{
	//	error = error_not_slitable;
	//	q->useslit = useslit = false;
	//	write_plugins();
	//}

	//if (false == useslit && NULL == q->beginPlugin && NULL == q->beginPluginEx)
	//{
	//	error = error_missing_entry;
	//	break;
	//}

	////---------------------------------------
	//// inititalize plugin

	//TRY
	//{
	//	if (useslit)
	//		if (q->beginPluginEx)
	//			i = q->beginPluginEx(hPlugin, hSlit);
	//		else
	//			i = q->beginSlitPlugin(hPlugin, hSlit);
	//	else
	//		if (q->beginPlugin)
	//			i = q->beginPlugin(hPlugin);
	//		else
	//			i = q->beginPluginEx(hPlugin, NULL);

	//	if (0 == i)
	//		q->hPlugin = hPlugin, q->inslit = useslit;
	//	else
	//		if (2 != i)
	//			error = error_fail_to_load;
	//}
	//EXCEPT
	//{
	//	error = error_crash_on_load;
	//}

	////---------------------------------------
	//// clean up after error

	//if (NULL == q->hPlugin)
	//{
	//	if (hPlugin) FreeLibrary(hPlugin);
	//	q->enabled = false;
	//	write_plugins();
	//}


	////---------------------------------------
	//// display errors

	//if (error) plugin_error(q, error);

	return 0;
}

int BBPlugin::getExportedFunctions() {
	if (hPlugin == NULL) {
		if (error.length() == 0) {
			error = "hPlugin is null";
		}
		return -1;
	}

	beginPlugin = (bPfp)GetProcAddress(hPlugin, "beginPlugin");
	endPlugin = (ePfp)GetProcAddress(hPlugin, "endPlugin");
	beginSlitPlugin = (bSPfp)GetProcAddress(hPlugin, "beginSlitPlugin");
	beginPluginEx = (bPEfp)GetProcAddress(hPlugin, "beginPluginEx");
	pluginInfo = (pIfp)GetProcAddress(hPlugin, "pluginInfo");

	if (endPlugin == NULL) {
		error = "Plugin doesn't have endPlugin()";
		return -1;
	}

	if (beginPlugin == NULL && beginSlitPlugin == NULL && beginPluginEx == NULL) {
		error = "Plugin doesn't have an entry point";
		return -1;
	}

	return 0;
}

int BBPlugin::runBegin() {
	if (hPlugin == NULL) {
		if (error.length() == 0) {
			error = "hPlugin is null";
		}
		return -1;
	}

	if (beginPlugin != NULL) {
		int ret = beginPlugin(hPlugin);
		if (ret != 0) {
			error = "beginPlugin returned an error";
		}

		return ret;
	} else if (beginPluginEx != NULL) {
		int ret = beginPluginEx(hPlugin, NULL);
		if (ret != 0) {
			error = "beginPluginEx returned an error";
		}

		return ret;
	}

	error = "Plugin doesn't have an entry point";
	return -1;
}

int BBPlugin::runEnd() {
	if (endPlugin != NULL) {
		endPlugin(hPlugin);
		FreeLibrary(hPlugin);
		hPlugin = NULL;
		return 0;
	}

	error = "Plugin doesn't have endPlugin()";
	return -1;
}

/*int BBPlugin::getPluginInfo() {
	if (pluginInfo != NULL) {
		return 0;
	}

	error = "Plugin doesn't have pluginInfo()";
	return -1;
}*/

std::string BBPlugin::getError() {
	return error;
}

BBPlugin::~BBPlugin() {
	if (hPlugin != NULL) {
		FreeLibrary(hPlugin);
	}
}

#ifdef _DEBUG
void BBPlugin::test_setPath(const std::string &path) {
	setPath(path);
}

void BBPlugin::test_setIsUsingSlit(bool usingSlit) {
	setIsUsingSlit(usingSlit);
}

int BBPlugin::test_load() {
	return load();
}

int BBPlugin::test_getExportedFunctions() {
	return getExportedFunctions();
}
#endif // _DEBUG
