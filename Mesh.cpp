#include "Mesh.h"

void Mesh::CalculateTangentsAndBinormals()
{
    tangents.resize(vertices.size());
    binormals.resize(vertices.size());

    for (size_t i = 0; i + 2 < vertices.size(); i += 3) {
        const glm::vec3& v0 = vertices[i];
        const glm::vec3& v1 = vertices[i + 1];
        const glm::vec3& v2 = vertices[i + 2];
        const glm::vec2& uv0 = texCoords[i];
        const glm::vec2& uv1 = texCoords[i + 1];
        const glm::vec2& uv2 = texCoords[i + 2];

        const glm::vec3 edge1 = v1 - v0;
        const glm::vec3 edge2 = v2 - v0;
        const glm::vec2 deltaUV1 = uv1 - uv0;
        const glm::vec2 deltaUV2 = uv2 - uv0;

        const float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        const glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        const glm::vec3 binormal = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

        tangents[i] = glm::normalize(tangent);
        tangents[i + 1] = glm::normalize(tangent);
        tangents[i + 2] = glm::normalize(tangent);

        binormals[i] = glm::normalize(binormal);
        binormals[i + 1] = glm::normalize(binormal);
        binormals[i + 2] = glm::normalize(binormal);
    }
}

void Mesh::CreateMeshObject()
{
	if (vertices.size() == 0) {
		__debugbreak(); // No vertices to create mesh from
	}

	for (size_t i = 0; i < vertices.size(); i++) {
		Vertex vertex{};
		vertex.pos = vertices[i];
        vertex.color = color;
        vertex.texCoord = texCoords[i];
		vertex.normal = normals[i];
		vertex.tangent = tangents[i];
		vertex.binormal = binormals[i];

		meshObject.push_back(vertex);
	}
}
