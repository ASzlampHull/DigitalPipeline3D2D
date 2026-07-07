#pragma once

#include "ParserBase.h"
#include "ModelParserDataTypes.h"


class OBJParser final {
private:
	std::string filePath;
	std::vector<ModelOBJ> parsedModelOBJs;

	const void HandleUseMtlLine(ModelOBJ& currentModelOBJ, const std::string& line) const;
	void HandleFaceLine(std::string& line, ModelOBJ& currentModelOBJ);
	void HandleTextureCoordLine(std::string& line);
	void HandleNormalLine(std::string& line);
	void HandleVertexLine(std::string& line);
	void HandleObjectLine(ModelOBJ& currentModelOBJ, const std::string& line, const std::string& configName);

	std::vector<glm::vec3> temporaryVertices;
	std::vector<glm::vec3> temporaryNormals;
	std::vector<glm::vec2> temporaryTexCoords;
public:
	OBJParser() = default;

	void ParseOBJFile(const std::string& filePath_, const std::string& configName);
	const std::vector<ModelOBJ>& GetParsedModelOBJs() const { return parsedModelOBJs; }
};