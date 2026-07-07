#include "OBJParser.h"

void OBJParser::ParseOBJFile(const std::string& filePath_, const std::string& configName)
{
	this->filePath = filePath_;

	temporaryVertices.clear();
	temporaryNormals.clear();
	temporaryTexCoords.clear();

	std::ifstream file(filePath);
	if (!file.is_open()) { throw std::runtime_error("Could not open OBJ file: " + filePath); }
	std::string line;
	ModelOBJ currentModelOBJ;
	while (std::getline(file, line)) {
		line = ParserBase::Trim(line);
		if (line.substr(0, 2) == "o ") {
			HandleObjectLine(currentModelOBJ, line, configName);
		}
		else if (line.substr(0, 2) == "v ") {
			HandleVertexLine(line);
		}
		else if (line.substr(0, 3) == "vn ") {
			HandleNormalLine(line);
		}
		else if (line.substr(0, 3) == "vt ") {
			HandleTextureCoordLine(line);
		}
		else if (line.substr(0, 2) == "f ") {
			HandleFaceLine(line, currentModelOBJ);
		}
		else if (line.substr(0, 7) == "usemtl ") {
			HandleUseMtlLine(currentModelOBJ, line);
		}
	}
	if (!currentModelOBJ.name.empty()) {
		parsedModelOBJs.push_back(currentModelOBJ);
	}
}

const void OBJParser::HandleUseMtlLine(ModelOBJ& currentModelOBJ, const std::string& line) const
{
	currentModelOBJ.materialName = ParserBase::Trim(line.substr(7));
}

//This function needs read the faces and extract the vertex, texture, and normal indices BUT also have the vertex/texture/normal to fit a Vulkan Vertex structure
void OBJParser::HandleFaceLine(std::string& line, ModelOBJ& currentModelOBJ)
{
    // Remove the 'f ' prefix
    std::istringstream s(line.substr(2));
    std::string vertexDef;

    // Temporary storage for this face's new indices
    std::vector<int> newVertexIndices;
    std::vector<int> newNormalIndices;
    std::vector<int> newTexCoordIndices;

    // For each vertex in the face
    while (s >> vertexDef) {
        size_t firstSlash = vertexDef.find('/');
        size_t secondSlash = vertexDef.find('/', firstSlash + 1);

        int vIdx = -1, vtIdx = -1, vnIdx = -1;

        // Parse vertex index
        if (firstSlash != std::string::npos) {
            vIdx = std::stoi(vertexDef.substr(0, firstSlash)) - 1;
        }
        else {
            vIdx = std::stoi(vertexDef) - 1;
        }

        // Parse texture index
        if (firstSlash != std::string::npos && secondSlash != std::string::npos && secondSlash > firstSlash + 1) {
            const std::string vtStr = vertexDef.substr(firstSlash + 1, secondSlash - firstSlash - 1);
            if (!vtStr.empty())
                vtIdx = std::stoi(vtStr) - 1;
        }

        // Parse normal index
        if (secondSlash != std::string::npos && secondSlash + 1 < vertexDef.size()) {
            const std::string vnStr = vertexDef.substr(secondSlash + 1);
			if (!vnStr.empty())
				vnIdx = std::stoi(vnStr) - 1;
        }

        // Add independent vertex, normal, texcoord to ModelOBJ
        if (vIdx >= 0 && vIdx < temporaryVertices.size())
            currentModelOBJ.vertices.push_back(temporaryVertices[vIdx]);
        if (vnIdx >= 0 && vnIdx < temporaryNormals.size())
            currentModelOBJ.normals.push_back(temporaryNormals[vnIdx]);
        if (vtIdx >= 0 && vtIdx < temporaryTexCoords.size())
            currentModelOBJ.texCoords.push_back(temporaryTexCoords[vtIdx]);

        // Store the new index (sequential, since we push_back above)
        const int newIndex = static_cast<int>(currentModelOBJ.vertices.size()) - 1;
        newVertexIndices.push_back(newIndex);
        newNormalIndices.push_back(newIndex);
        newTexCoordIndices.push_back(newIndex);
    }

    // Triangulate using fan method (for n-gons)
    if (newVertexIndices.size() > 3) {
        for (size_t i = 1; i + 1 < newVertexIndices.size(); ++i) {
            currentModelOBJ.vertexIndices.push_back(newVertexIndices[0]);
            currentModelOBJ.vertexIndices.push_back(newVertexIndices[i]);
            currentModelOBJ.vertexIndices.push_back(newVertexIndices[i + 1]);

            currentModelOBJ.normalIndices.push_back(newNormalIndices[0]);
            currentModelOBJ.normalIndices.push_back(newNormalIndices[i]);
            currentModelOBJ.normalIndices.push_back(newNormalIndices[i + 1]);

            currentModelOBJ.texCoordIndices.push_back(newTexCoordIndices[0]);
            currentModelOBJ.texCoordIndices.push_back(newTexCoordIndices[i]);
            currentModelOBJ.texCoordIndices.push_back(newTexCoordIndices[i + 1]);
        }
    }

    if (newVertexIndices.size() == 3) {
        currentModelOBJ.vertexIndices.push_back(newVertexIndices[0]);
        currentModelOBJ.vertexIndices.push_back(newVertexIndices[1]);
        currentModelOBJ.vertexIndices.push_back(newVertexIndices[2]);

        currentModelOBJ.normalIndices.push_back(newNormalIndices[0]);
        currentModelOBJ.normalIndices.push_back(newNormalIndices[1]);
        currentModelOBJ.normalIndices.push_back(newNormalIndices[2]);

        currentModelOBJ.texCoordIndices.push_back(newTexCoordIndices[0]);
        currentModelOBJ.texCoordIndices.push_back(newTexCoordIndices[1]);
        currentModelOBJ.texCoordIndices.push_back(newTexCoordIndices[2]);
    }
}

void OBJParser::HandleTextureCoordLine(std::string& line)
{
	std::istringstream s(line.substr(3));
	glm::vec2 texCoord;
	s >> texCoord.x >> texCoord.y;
	temporaryTexCoords.push_back(texCoord);
}

void OBJParser::HandleNormalLine(std::string& line)
{
	std::istringstream s(line.substr(3));
	glm::vec3 normal;
	s >> normal.x >> normal.y >> normal.z;
	temporaryNormals.push_back(normal);
}

void OBJParser::HandleVertexLine(std::string& line)
{
	std::istringstream s(line.substr(2));
	glm::vec3 vertex;
	s >> vertex.x >> vertex.y >> vertex.z;
	temporaryVertices.push_back(vertex);
}

void OBJParser::HandleObjectLine(ModelOBJ& currentModelOBJ, const std::string& line, const std::string& configName)
{
	if (!currentModelOBJ.name.empty()) {
		parsedModelOBJs.push_back(currentModelOBJ);
		currentModelOBJ = ModelOBJ();
	}
	currentModelOBJ.name = ParserBase::Trim(line.substr(2));
    currentModelOBJ.configName = configName;
}
