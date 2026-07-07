#include "ModelLoader.h"

void ModelLoader::ParseModels()
{
	for (const auto& modelPair : configData.models) {
		const ModelStats& modelData = modelPair.second;
		const std::string modelName = modelData.name;
		const std::string& modelPath = modelData.modelPath;
		const std::string& materialPath = modelData.materialPath;

		objParser.ParseOBJFile(modelPath, modelName);
		mtlParser.ParseMTLFile(materialPath);
	}
}

void ModelLoader::LoadModels()
{
	for (int modelCount = 0; modelCount < objParser.GetParsedModelOBJs().size(); modelCount++) {

		ModelData modelData;
		modelData.name = objParser.GetParsedModelOBJs()[modelCount].name;
		modelData.objData = objParser.GetParsedModelOBJs()[modelCount];
		
		// Find corresponding MTL data by name
		const std::string mtlName = modelData.objData.materialName;
		modelData.mtlData = mtlParser.GetParsedModelMTLs().at(mtlName);
		if (modelData.mtlData.name != mtlName) {
			__debugbreak(); // MTL name does not match OBJ material name
		}

		// Set transformation data from config
		const ModelStats& modelConfigData = configData.models.at(objParser.GetParsedModelOBJs()[modelCount].configName);
		modelData.position = modelConfigData.position;
		modelData.rotation = modelConfigData.rotation;
		modelData.scale = modelConfigData.scale;
		loadedModels.push_back(modelData);
	}
}
