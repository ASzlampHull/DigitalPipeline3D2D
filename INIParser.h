#pragma once
#include <fstream>
#include <sstream>
#include "ParserBase.h"

using Section = std::unordered_map<std::string, std::string>;
using IniData = std::unordered_map<std::string, Section>;

class INIParser final {
private:
	IniData iniData;
	std::string filename;

public:
	INIParser() = default;
	~INIParser() = default;
	explicit INIParser(const std::string& filename_) : filename(filename_) {
		ParseINIFile();
	}
	void ParseINIFile();

	std::string GetData(const std::string& section, const std::string& key) { return iniData[section][key]; }
	const std::string GetTitle() { return iniData["Title"]["Title"]; }
	int GetIntData(const std::string& section, const std::string& key) { return std::stoi(iniData[section][key]); }
	float GetFloatData(const std::string& section, const std::string& key) { return std::stof(iniData[section][key]); }
	glm::vec3 GetVec3Data(const std::string& section, const std::string& key);
	void GetKeys(const std::string& section, std::vector<std::string>& keys_);
};