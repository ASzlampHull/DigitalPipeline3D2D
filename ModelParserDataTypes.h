#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using IndicesVector = std::vector<uint32_t>;

struct ModelOBJ {
	std::string configName = ""; // custom field for internal use
	std::string name = ""; // o
	std::vector<glm::vec3> vertices = {}; // v
	std::vector<glm::vec3> normals = {}; // vn
	std::vector<glm::vec2> texCoords = {}; // vt
	IndicesVector vertexIndices = {}; // f
	IndicesVector normalIndices = {}; // f
	IndicesVector texCoordIndices = {}; // f
	std::string materialName = ""; // usemtl
};

struct ModelMTL {
	std::string name = "";         // newmtl
	std::string texturePath = "";  // map_Kd

	glm::vec3 ambientColor = glm::vec3(0.0f);    // Ka
	glm::vec3 diffuseColor = glm::vec3(0.0f);    // Kd
	glm::vec3 specularColor = glm::vec3(0.0f);   // Ks
	glm::vec3 emissionColor = glm::vec3(0.0f);   // Ke

	float shininess = 0.0f;          // Ns
	float refractionIndex = 1.0f;    // Ni
	float transparency = 1.0f;      // d

	unsigned int illuminationModel = 0; // illum
};

struct ModelData {
	std::string name = "";
	ModelOBJ objData = {};
	ModelMTL mtlData = {};
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
};

using ModelVector = std::vector<ModelData>;