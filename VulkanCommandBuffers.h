#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "RenderDataTypes.h"
#include <stdexcept>
class VulkanCommandBuffers final {
private:
	CommandBuffersVulkan commandBuffersVulkan = {};
	const CoreVulkan* coreVulkan = nullptr;
	const CommandPoolVulkan* commandPoolVulkan = nullptr;

	void CreateCommandBuffers();
public:
	VulkanCommandBuffers() = default;
	~VulkanCommandBuffers() = default;
	VulkanCommandBuffers(const CoreVulkan* coreVulkan_, const CommandPoolVulkan* commandPoolVulkan_)
		: coreVulkan(coreVulkan_), commandPoolVulkan(commandPoolVulkan_)
	{
		CreateCommandBuffers();
	}
	VulkanCommandBuffers(const VulkanCommandBuffers& other) = default;
	VulkanCommandBuffers& operator=(const VulkanCommandBuffers& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->commandPoolVulkan = rhs.commandPoolVulkan;
			this->commandBuffersVulkan = rhs.commandBuffersVulkan;
		}
		return *this;
	}

	const CommandBuffersVulkan& GetCommandBuffersVulkan() const { return commandBuffersVulkan; };
};