#pragma once
#include <vector>
#include "ModelParserDataTypes.h"
#include "RenderDataTypes.h"

class Mesh final {
private:
	MeshObject meshObject = {};
	std::vector<glm::vec3> vertices = {};
	glm::vec3 color = glm::vec3(0.0f, 0.0f, 1.0f);
	std::vector<glm::vec3> normals = {};
	std::vector<glm::vec2> texCoords = {};
	std::vector<glm::vec3> tangents = {};
	std::vector<glm::vec3> binormals = {};
	std::vector<unsigned int> triangleIds = {};
	IndicesVector vertexIndices = {};
	IndicesVector normalIndices = {};
	IndicesVector texCoordIndices = {};

	void CalculateTangentsAndBinormals();
	void CreateMeshObject();
public:
	Mesh() = default;
	~Mesh() = default;
	Mesh(const ModelOBJ& objData, const ModelMTL& mtlData)
		: vertices(objData.vertices), color(mtlData.ambientColor), normals(objData.normals), texCoords(objData.texCoords), 
		vertexIndices(objData.vertexIndices), normalIndices(objData.normalIndices), texCoordIndices(objData.texCoordIndices) {
		CalculateTangentsAndBinormals();
		CreateMeshObject();
	}

	const MeshObject& GetMeshObject() const { return meshObject; };
	const IndicesVector& GetVertexIndices() const { return vertexIndices; };
	const std::vector<glm::vec3>& GetVertices() const { return vertices; };
};