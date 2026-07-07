#pragma once

#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
class VulkanFrameSync final {
private:
	FrameSyncVulkan frameSyncVulkan = {};
	const CoreVulkan* coreVulkan = nullptr;

	void CreateSyncObjects();
public:
	VulkanFrameSync() = default;
	~VulkanFrameSync() = default;
	explicit VulkanFrameSync(const CoreVulkan* coreVulkan_)
		: coreVulkan(coreVulkan_)
	{
		CreateSyncObjects();
	}
	VulkanFrameSync(const VulkanFrameSync& other) = default;
	VulkanFrameSync& operator=(const VulkanFrameSync& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->frameSyncVulkan = rhs.frameSyncVulkan;
		}
		return *this;
	}

	const FrameSyncVulkan& GetFrameSyncVulkan() const { return frameSyncVulkan; };
	void Cleanup();
};