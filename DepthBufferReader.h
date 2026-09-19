#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"

// This class reads depth buffer data from Vulkan and provides access to depth values at specific pixels
class DepthBufferReader
{
private:
	const CoreVulkan* coreVulkan = nullptr;
	UniformVulkan depthBuffers;
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t frameCount = 0;

	void Initialise();

public:
	DepthBufferReader() = default;
	~DepthBufferReader() = default;
	DepthBufferReader(const DepthBufferReader& other) = default;
	DepthBufferReader(const CoreVulkan* coreVulkan_, uint32_t width_, uint32_t height_) : coreVulkan(coreVulkan_), width(width_), height(height_)
	{
		Initialise();
	}
	DepthBufferReader& operator=(const DepthBufferReader& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->depthBuffers = rhs.depthBuffers;
			this->width = rhs.width;
			this->height = rhs.height;
			this->frameCount = rhs.frameCount;
		}
		return *this;
	}
		
	// Read depth at a pixel (after fence wait)
	float DepthAt(uint32_t frameIndex, uint32_t x, uint32_t y) const {
		const float* depthData = static_cast<const float*>(depthBuffers.uniformBuffersMapped[frameIndex]);
		return depthData[y * width + x];
	}
	// Read depth row (after fence wait)
	const float* DepthRow(uint32_t frameIndex, uint32_t y) const {
		const float* depthData = static_cast<const float*>(depthBuffers.uniformBuffersMapped[frameIndex]);
		return depthData + y * width;
	}
	void RecordDepthCopy(VkCommandBuffer commandBuffer, uint32_t frameIndex, VkImage depthImage);

	void CleanUp();
};