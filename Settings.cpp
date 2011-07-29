#include "Settings.h"
#include "Files.h"
#include "BBColor.h"
#include "../Litestep24x/lsapi/lsapi.h"
#include <sstream>
#include <fstream>

Settings::Settings() {
	readRCValues();
}

void Settings::readRCValues() {
	char buff[MAX_PATH];

	GetRCString("bbPlugLdr.styleRCPath", buff, "$themedir$bbstyle.rc", MAX_PATH);
	settings["styleRCPath"] = std::string(buff);

	GetRCString("blackbox.editor:", buff, "notepad.exe", MAX_PATH);
	settings["blackbox.editor:"] = std::string(buff);
}

std::string Settings::getString(const std::string& name) {
	return settings[name];
}

int Settings::getInt(const std::string& name) {
	int tmp;
	std::istringstream iss(settings[name]);
	iss >> tmp;
	return tmp;
}

bool Settings::getBool(const std::string& name) {
	return settings[name] == "true";
}

bool Settings::readBool(const std::string& fileName, const std::string& key, bool defaultBool) {
	return GetRCBoolDef(key.c_str(), defaultBool);
}

int Settings::readInt(const std::string& fileName, const std::string& key, int defaultInt) {
	return GetRCInt(key.c_str(), defaultInt);
}

const std::string& Settings::readString(const std::string& fileName, const std::string& key, const std::string& defaultString) {
	// All string values read must be stored in the local settings map to make sure
	// that the pointer to the string value stays valid until the module is unloaded
	std::string &value = settings[key];

	if (value == "") {
		char buf[MAX_PATH];
		GetRCString(key.c_str(), buf, defaultString.c_str(), MAX_PATH);
		value = std::string(buf); // Will also update settings[key] because of the reference
	}

	return value;
}

COLORREF Settings::readColor(const std::string& fileName, const std::string& key, const std::string& defaultString) {
	std::string &value = settings[key];

	if (value == "") {
		char buf[MAX_PATH];
		GetRCString(key.c_str(), buf, defaultString.c_str(), MAX_PATH);
		value = std::string(buf); // Will also update settings[key] because of the reference
	}

	BBColor bbc;
	return bbc.readColorFromString(value);
}

void Settings::writeString(const std::string& fileName, const std::string& key, const std::string& value) {
	Files f;
	if (!f.fileExists(fileName)) {
		return;
	}

	// Read through the file. Delete the key+value if it's already there

	std::ifstream inFile(fileName.c_str());

	if (!inFile.is_open()) {
		return;
	}

	std::string line;
	std::string tmp;
	std::ostringstream file;

	while (!inFile.eof()) {
		getline(inFile, line);
		tmp = line;
		tmp.erase(0, tmp.find_first_not_of(" \t\n\r"));

		if (tmp.find(key, 0) != 0 && tmp.length() > 0) {
			file << line << "\n";
		}
	}

	inFile.close();

	// Append the new key+value to the file

	std::ofstream outFile(fileName.c_str());

	if (!outFile.is_open()) {
		return;
	}

	outFile << file.str() << key << " " << value << "\n";

	outFile.close();
}

void Settings::writeBool(const std::string& fileName, const std::string& key, bool value) {
	writeString(fileName, key, value ? "true" : "false");
}

void Settings::writeInt(const std::string& fileName, const std::string& key, int value) {
	std::ostringstream oss;
	oss << value;
	writeString(fileName, key, oss.str());
}

void Settings::writeColor(const std::string& fileName, const std::string& key, COLORREF value) {
	BBColor bbc;
	writeString(fileName, key, bbc.colorrefToString(value));
}
