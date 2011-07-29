#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <string>
#include <map>
#include <windows.h>

class Settings {
private:
	std::map<std::string, std::string> settings;
	void readRCValues();
public:
	Settings();
	std::string getString(const std::string& name);
	int getInt(const std::string& name);
	bool getBool(const std::string& name);

	bool readBool(const std::string& fileName, const std::string& key, bool defaultBool);
	int readInt(const std::string& fileName, const std::string& key, int defaultInt);
	const std::string& readString(const std::string& fileName, const std::string& key, const std::string& defaultString);
	COLORREF readColor(const std::string& fileName, const std::string& key, const std::string& defaultString);

	void writeString(const std::string& fileName, const std::string& key, const std::string& value);
	void writeBool(const std::string& fileName, const std::string& key, bool value);
	void writeInt(const std::string& fileName, const std::string& key, int value);
	void writeColor(const std::string& fileName, const std::string& key, COLORREF value);
};

#endif