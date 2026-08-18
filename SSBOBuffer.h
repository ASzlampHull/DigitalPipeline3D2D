#pragma once
#include "VulkanExternals.h"
#include "VulkanDataTypes.h"


class SSBOBuffer
{
private:
	// Constants
	const uint32_t numElements = 10000;

	// Vulkan handlers
	DescriptorVulkan descriptorVulkan = {};
	const CoreVulkan* coreVulkan = nullptr;
	const PipelineVulkan* pipelineVulkan = nullptr;

	// Vulkan handles and resources for the SSBO buffer
	VkBuffer ssboBuffer;
	VkDeviceSize ssboBufferSize;
	VkDeviceMemory ssboBufferMemory;

	void CreateSSBOBuffer();
	void CreateDescriptorPool();
	//void CreateDescriptorSetLayout();
	void CreateDescriptorSet();

public:
	SSBOBuffer() = default;
	~SSBOBuffer() = default;
	SSBOBuffer(const CoreVulkan* coreVulkan_, const PipelineVulkan* pipelineVulkan_)
		: coreVulkan(coreVulkan_),
		pipelineVulkan(pipelineVulkan_)
	{
		CreateSSBOBuffer();
		CreateDescriptorPool();
		//CreateDescriptorSetLayout();
		CreateDescriptorSet();
	}
	SSBOBuffer(const SSBOBuffer& other) = default;
	SSBOBuffer& operator=(const SSBOBuffer& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->descriptorVulkan = rhs.descriptorVulkan;
			this->pipelineVulkan = rhs.pipelineVulkan;
			this->ssboBuffer = rhs.ssboBuffer;
			this->ssboBufferSize = rhs.ssboBufferSize;
			this->ssboBufferMemory = rhs.ssboBufferMemory;
		}
		return *this;
	}

	void DEBUG_PrintSSBOBufferInfo();

	VkBuffer GetSSBOBuffer() const { return ssboBuffer; }
	VkDeviceSize GetSSBOBufferSize() const { return ssboBufferSize; }
	VkDeviceMemory GetSSBOBufferMemory() const { return ssboBufferMemory; }

	const uint32_t GetNumElements() const { return numElements; }
	const DescriptorVulkan& GetDescriptorVulkan() const { return descriptorVulkan; };

	void CleanUp();
};