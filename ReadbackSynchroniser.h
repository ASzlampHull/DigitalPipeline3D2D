#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"

// Class that manages fences for synchronizing readback operations in Vulkan.
// For race conditions with the CPU and GPU.
class ReadbackSynchroniser
{
private:
	const CoreVulkan* coreVulkan = nullptr;
	uint32_t frameCount = 0;
	std::vector<VkFence> fences;

	void Initialise();

public:
	ReadbackSynchroniser() = default;
	~ReadbackSynchroniser() = default;
	ReadbackSynchroniser(const CoreVulkan* coreVulkan_, uint32_t frameCount_) : coreVulkan(coreVulkan_), frameCount(frameCount_)
	{
		Initialise();
	}
	ReadbackSynchroniser& operator=(const ReadbackSynchroniser& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->frameCount = rhs.frameCount;
			this->fences = rhs.fences;
		}
		return *this;
	}

	void BeginReadback(VkQueue queue, VkCommandBuffer commandBuffer, uint32_t frameIndex);
	bool WaitForReadback(uint32_t frameIndex, uint64_t timeoutNs = UINT64_MAX);
	bool IsReadbackComplete(uint32_t frameIndex) const;
	void ResetFence(uint32_t frameIndex);

	// Accessors
	VkFence GetFenceForFrame(uint32_t frameIndex) const { return fences[frameIndex]; }

	void CleanUp();
};