#pragma once

#include "ModelParserDataTypes.h"
#include "ConfigDataTypes.h"
#include "OBJParser.h"
#include "MTLParser.h"

class ModelLoader final {
private:
	// Private methods for parsing .obj files can be added here
	ModelVector loadedModels;
	OBJParser objParser;
	MTLParser mtlParser;
	ConfigData configData;

	void ParseModels();
	void LoadModels();
public:
	ModelLoader() = default;
	~ModelLoader() = default;
	explicit ModelLoader(const ConfigData& configData_) : configData(configData_) {
		ParseModels();
		LoadModels(); 
	};	

	const ModelVector& GetLoadedModels() const { return loadedModels; };

	ModelLoader& operator=(const ModelLoader& rhs)
	{
		if (this != &rhs) {
			this->loadedModels = rhs.loadedModels;
			this->objParser = rhs.objParser;
			this->mtlParser = rhs.mtlParser;
			this->configData = rhs.configData;
		}
		return *this;
	}
};