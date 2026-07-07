#pragma once
#include "VulkanDataTypes.h"
#include "RenderDataTypes.h"
#include "ModelParserDataTypes.h"
#include "Model.h"
#include "Texture.h"
#include "VulkanBuffers.h"
#include "VulkanDescriptor.h"

class Particles final {
private:
	const uint32_t MAX_PARTICLES = 200;
	MeshObject particleMeshObject;
	IndicesVector vertexIndices;
	Texture texture = {};
	VulkanBuffers vulkanBuffers;
	const ModelBuffersVulkan* modelBuffersVulkan = nullptr;
	VulkanDescriptor vulkanDescriptor = {};
	const DescriptorVulkan* descriptorVulkan = nullptr;
	Transformations transformations = {};

	void InitialiseParticles();
	void CreateParticleMesh();
	void SetTransformations();
	void AssignTexures(const std::string texture_);
	void CreateVertexIndexBuffers(const CoreVulkan* coreVulkan, const CommandPoolVulkan* commandPoolVulkan);
	void CreateTexture(const CoreVulkan* coreVulkan_, const CommandPoolVulkan* commandPoolVulkan_, const VkSampler* texSampler);
	void CreateDescriptor(const CoreVulkan* coreVulkan_, const ModelBuffersVulkan* modelBuffersVulkan_, const PipelineVulkan* pipelineVulkan_, const UniformVulkan& uniformBufferObject);
public:
	explicit Particles() {
		InitialiseParticles();
	}
	~Particles() = default;
	Particles(const Particles& other) = default;

	Particles& operator=(const Particles& other)
	{
		if (this != &other) {
			this->particleMeshObject = other.particleMeshObject;
			this->vertexIndices = other.vertexIndices;
			this->texture = other.texture;
			this->vulkanBuffers = other.vulkanBuffers;
			this->modelBuffersVulkan = other.modelBuffersVulkan;
			this->vulkanDescriptor = other.vulkanDescriptor;
			this->descriptorVulkan = other.descriptorVulkan;
			this->transformations = other.transformations;
		}
		return *this;
	}

	void CreateVulkanResources(
		const CoreVulkan* coreVulkan_,
		const CommandPoolVulkan* commandPoolVulkan_,
		const PipelineVulkan* pipelineVulkan_,
		const UniformVulkan& uniformBufferObject,
		const VkSampler* texSampler);
	void UpdatePushConstants(VkCommandBuffer commandBuffer, const PipelineVulkan* pipelineVulkan) const;

	const MeshObject GetParticleMeshObject() const { return particleMeshObject; };
	const IndicesVector GetVertexIndices() const { return vertexIndices; };
	const ModelBuffersVulkan* GetModelBuffersVulkan() const { return modelBuffersVulkan; };
	const DescriptorVulkan* GetDescriptorVulkan() const { return descriptorVulkan; };

	const void CleanupBuffersVI() const { vulkanBuffers.CleanupBuffersVI(); };
	const void CleanupTexDest() const { texture.Cleanup(); vulkanDescriptor.Cleanup(); };
};