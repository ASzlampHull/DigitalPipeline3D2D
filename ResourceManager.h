#pragma once
//Pass in the loaded model data from ModelLoader to create Model instances
#include "Model.h"
#include "Particles.h"
#include "ModelParserDataTypes.h"
#include "ModelLoader.h"

class ResourceManager final {
private:
	std::unordered_map<std::string, Model> models;
	Particles fireParticles;
	ModelLoader modelLoader;
	MeshObject allMeshObjects;
	IndicesVector allMeshIndices;
	std::unordered_map<std::string, VkSampler> textureSamplers;

	void CreateModels();
	void CreateParticles();
	void CreateMainTextureSampler(const CoreVulkan* coreVulkan);
public:
	ResourceManager() = default;
	~ResourceManager() = default;
	explicit ResourceManager(const ConfigData& configData) : modelLoader(configData) 
	{
		CreateModels();
	}

	void CreateTextures(const CoreVulkan* coreVulkan, const CommandPoolVulkan* commandPoolVulkan, const PipelineVulkan* pipelineVulkan, UniformVulkan& uniformBufferObject);
	void CleanupTextures(const CoreVulkan* coreVulkan);
	void CreateVertexIndexBuffers(const CoreVulkan* coreVulkan, const CommandPoolVulkan* commandPoolVulkan);
	void CleanupBuffersVI();

	const std::unordered_map<std::string, Model>& GetModels() { return models; };
	Model& GetModelRef(const std::string& name) { return models.at(name); };
	const Particles& GetParticles() { return fireParticles; };
	const VkDescriptorPool GetMainDescriptorPool() const;
};