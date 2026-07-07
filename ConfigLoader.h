#pragma once
#include <vector>
#include "ConfigDataTypes.h"
#include "INIParser.h"

class ConfigLoader final{
private:
	INIParser iniParser;
	ConfigData configData;
public:
	ConfigLoader() = default;
	~ConfigLoader() = default;
	explicit ConfigLoader(const std::string& filename) : iniParser(filename) {
		CreateConfigData();
	};


	void CreateConfigData();
	void CreateResolution();
	void CreateTitle();
	void CreateModels();
	void CreateDirectionalLight();
	void CreateSunMoonDirections();
	void CreateSeasonalTimings();

	const ConfigData& GetConfigData() const { return configData; };
};