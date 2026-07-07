#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "RenderDataTypes.h"
#include <stdexcept>


class VulkanFramebuffers final {
private:
	CommandPoolVulkan commandPoolVulkan;
	DepthVulkan depthVulkan;
	SwapChainVulkan* swapChainVulkan;
	const CoreVulkan* coreVulkan;
	const PipelineVulkan* pipelineVulkan;

	void InitialiseFramebuffers();
	void CreateCommandPool();
	const bool hasStencilComponent(VkFormat format) const { return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT; }
public:
	VulkanFramebuffers() = default;
	~VulkanFramebuffers() = default;
	VulkanFramebuffers(const CoreVulkan* coreVulkan_, SwapChainVulkan* swapChainVulkan_, const PipelineVulkan* pipelineVulkan_)
		: commandPoolVulkan(),
		depthVulkan(),
		swapChainVulkan(swapChainVulkan_),
		coreVulkan(coreVulkan_),
		pipelineVulkan(pipelineVulkan_)
	{
		InitialiseFramebuffers();
	}
	VulkanFramebuffers(const VulkanFramebuffers& other) = default;
	VulkanFramebuffers& operator=(const VulkanFramebuffers& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->swapChainVulkan = rhs.swapChainVulkan;
			this->pipelineVulkan = rhs.pipelineVulkan;
			this->commandPoolVulkan = rhs.commandPoolVulkan;
			this->depthVulkan = rhs.depthVulkan;
		}
		return *this;
	}
	const CommandPoolVulkan& GetCommandPoolVulkan() const { return commandPoolVulkan; };

	void CreateDepthResources();
	void CreateFramebuffers();
	const void CleanupDepth() const;
	const void CleanupCommandPool() const { vkDestroyCommandPool(coreVulkan->device, commandPoolVulkan.commandPool, nullptr); };
};