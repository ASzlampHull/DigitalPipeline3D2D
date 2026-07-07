#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "RenderDataTypes.h"

class VulkanDescriptor final {
private:
	DescriptorVulkan descriptorVulkan = {};
	UniformVulkan uniformBufferObject = {};
	const CoreVulkan* coreVulkan = nullptr;
	const ModelBuffersVulkan* modelBuffersVulkan = nullptr;
	const PipelineVulkan* pipelineVulkan = nullptr;
	const TextureVulkan* textureVulkan = nullptr;

	void InitialiseDescriptor();
	void CreateDescriptorPool();
	void CreateDescriptorSets();
public:
	VulkanDescriptor() = default;
	~VulkanDescriptor() = default;
	explicit VulkanDescriptor(
		const CoreVulkan* coreVulkan_,
		const ModelBuffersVulkan* modelBuffersVulkan_,
		const PipelineVulkan* pipelineVulkan_,
		const TextureVulkan* textureVulkan_,
		const UniformVulkan& uniformBufferObject_)
		: uniformBufferObject(uniformBufferObject_),
		coreVulkan(coreVulkan_),
		modelBuffersVulkan(modelBuffersVulkan_),
		pipelineVulkan(pipelineVulkan_),
		textureVulkan(textureVulkan_)
	{
		InitialiseDescriptor();
	}
	VulkanDescriptor(const VulkanDescriptor& other) = default;
	VulkanDescriptor& operator=(const VulkanDescriptor& rhs)
	{
		if (this != &rhs) {
			coreVulkan = rhs.coreVulkan;
			modelBuffersVulkan = rhs.modelBuffersVulkan;
			pipelineVulkan = rhs.pipelineVulkan;
			textureVulkan = rhs.textureVulkan;
			uniformBufferObject = rhs.uniformBufferObject;
			descriptorVulkan = rhs.descriptorVulkan;
		}
		return *this;
	}

	const DescriptorVulkan& GetDescriptorVulkan() const { return descriptorVulkan; };
	const void Cleanup() const { vkDestroyDescriptorPool(coreVulkan->device, descriptorVulkan.descriptorPool, nullptr); };
};

