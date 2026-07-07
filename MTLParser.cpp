#include "MTLParser.h"

void MTLParser::ParseMTLFile(const std::string& filePath_)
{
	this->filePath = filePath_;
	std::ifstream file(filePath);
	if (!file.is_open()) { throw std::runtime_error("Could not open MTL file: " + filePath); }
	std::string line;
	ModelMTL currentModelMTL;
	DefaultMaterialValues(currentModelMTL);
	while (std::getline(file, line)) {
		line = ParserBase::Trim(line);
		if (line.substr(0, 7) == "newmtl ") {
			if (!currentModelMTL.name.empty()) {
				parsedModelMTLs[currentModelMTL.name] = currentModelMTL;
				currentModelMTL = ModelMTL();
			}
			currentModelMTL.name = ParserBase::Trim(line.substr(7));
		}
		else if (line.substr(0, 3) == "Ns ") {
			currentModelMTL.shininess = std::stof(ParserBase::Trim(line.substr(3)));
		}
		else if (line.substr(0, 3) == "Ka ") {
			std::istringstream s(line.substr(3));
			s >> currentModelMTL.ambientColor.x >> currentModelMTL.ambientColor.y >> currentModelMTL.ambientColor.z;
		}
		else if (line.substr(0, 3) == "Kd ") {
			std::istringstream s(line.substr(3));
			s >> currentModelMTL.diffuseColor.x >> currentModelMTL.diffuseColor.y >> currentModelMTL.diffuseColor.z;
		}
		else if (line.substr(0, 3) == "Ks ") {
			std::istringstream s(line.substr(3));
			s >> currentModelMTL.specularColor.x >> currentModelMTL.specularColor.y >> currentModelMTL.specularColor.z;
		}
		else if (line.substr(0, 3) == "Ke ") {
			std::istringstream s(line.substr(3));
			s >> currentModelMTL.emissionColor.x >> currentModelMTL.emissionColor.y >> currentModelMTL.emissionColor.z;
		}
		else if (line.substr(0, 3) == "Ni ") {
			currentModelMTL.refractionIndex = std::stof(ParserBase::Trim(line.substr(3)));
		}
		else if (line.substr(0, 2) == "d ") {
			currentModelMTL.transparency = std::stof(ParserBase::Trim(line.substr(2)));
		}
		else if (line.substr(0, 6) == "illum ") {
			currentModelMTL.illuminationModel = static_cast<unsigned int>(std::stoi(ParserBase::Trim(line.substr(6))));
		}
		else if (line.substr(0, 7) == "map_Kd ") {
			const std::size_t pos = line.find("textures/");
			if (pos != std::string::npos)
				currentModelMTL.texturePath = line.substr(pos);
		}
	}
	if (!currentModelMTL.name.empty()) {
		parsedModelMTLs[currentModelMTL.name] = currentModelMTL;
	}
}

const void MTLParser::DefaultMaterialValues(ModelMTL& currentModelMTL) const
{
	currentModelMTL.ambientColor = { 0.1f, 0.1f, 0.1f };
	currentModelMTL.diffuseColor = { 0.9f, 0.7f, 0.9f };
	currentModelMTL.specularColor = { 1.0f, 1.0f, 1.0f };
	currentModelMTL.emissionColor = { 0.0f, 0.0f, 0.0f };
	currentModelMTL.refractionIndex = 1.0f;
	currentModelMTL.transparency = 1.0f;
	currentModelMTL.shininess = 32.0f;
}
