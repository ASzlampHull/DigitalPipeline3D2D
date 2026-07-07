#include "ConfigLoader.h"

void ConfigLoader::CreateConfigData()
{
	CreateTitle();
	CreateResolution();
	CreateModels();
	CreateDirectionalLight();
	CreateSunMoonDirections();
	CreateSeasonalTimings();
}

void ConfigLoader::CreateResolution()
{
	configData.resolution.width = iniParser.GetIntData("Resolution", "width");
	configData.resolution.height = iniParser.GetIntData("Resolution", "height");
}

void ConfigLoader::CreateTitle()
{
	configData.title = iniParser.GetTitle();
}

void ConfigLoader::CreateModels()
{
	const std::string mOBJ = "ModelOBJ", mMTL = "ModelMTL", mPos = "ModelPosition", mRot = "ModelRotation", mScal = "ModelScale";

	std::vector<std::string> modelNames;
	iniParser.GetKeys(mOBJ, modelNames);

	for (const auto& modelName : modelNames) {
		ModelStats modelStats;
		modelStats.name = modelName;
		modelStats.modelPath = iniParser.GetData(mOBJ, modelName);
		modelStats.materialPath = iniParser.GetData(mMTL, modelName);
		modelStats.position = iniParser.GetVec3Data(mPos, modelName);
		modelStats.rotation = iniParser.GetVec3Data(mRot, modelName);
		modelStats.scale = iniParser.GetVec3Data(mScal, modelName);

		configData.models[modelName] = modelStats;
	}
}

void ConfigLoader::CreateDirectionalLight()
{
	const std::string section = "DirectionalLight";
	configData.directionalLight.direction = iniParser.GetVec3Data(section, "direction");
	configData.directionalLight.color = iniParser.GetVec3Data(section, "color");
	configData.directionalLight.intensity = iniParser.GetFloatData(section, "intensity");
}

void ConfigLoader::CreateSunMoonDirections()
{
	const std::string section = "SunMoonDirections";
	configData.sunMoonDirections.sunDirection = iniParser.GetVec3Data(section, "sun");
	configData.sunMoonDirections.moonDirection = iniParser.GetVec3Data(section, "moon");
}

void ConfigLoader::CreateSeasonalTimings()
{
	const std::string section = "SeasonalTimings";
	configData.seasonalTimings.dayDuration = iniParser.GetFloatData(section, "day");
	configData.seasonalTimings.nightDuration = iniParser.GetFloatData(section, "night");
	configData.seasonalTimings.drySeasonDuration = iniParser.GetFloatData(section, "dry");
	configData.seasonalTimings.snowSeasonDuration = iniParser.GetFloatData(section, "snowy");
	configData.seasonalTimings.wetSeasonDuration = iniParser.GetFloatData(section, "wet");
}
