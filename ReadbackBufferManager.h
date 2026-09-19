#pragma once
#include "VulkanDataTypes.h"
#include "FeatureReadbackTypes.h"
#include "VulkanExternals.h"

// This class manages Vulkan readback buffers for transferring image data from GPU to CPU
class ReadbackBufferManager
{
private:
	const CoreVulkan* coreVulkan = nullptr;
	UniformVulkan readbackBuffers;
	VkDeviceSize bufferSize;
	uint32_t width = 0;
	uint32_t height = 0;
	uint32_t frameCount = 0;

	void Initialise(VkDeviceSize bytesPerPixel);

public:
	ReadbackBufferManager() = default;
	~ReadbackBufferManager() = default;
	ReadbackBufferManager(const CoreVulkan* coreVulkan_, 
		uint32_t width_, 
		uint32_t height_, 
		uint32_t frameCount_, 
		VkDeviceSize bytesPerPixel) : coreVulkan(coreVulkan_), width(width_), height(height_), frameCount(frameCount_)
	{
		Initialise(bytesPerPixel);
	}
	ReadbackBufferManager(const ReadbackBufferManager& other) = default;
	ReadbackBufferManager& operator=(const ReadbackBufferManager& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->readbackBuffers = rhs.readbackBuffers;
			this->bufferSize = rhs.bufferSize;
			this->width = rhs.width;
			this->height = rhs.height;
			this->frameCount = rhs.frameCount;
		}
		return *this;
	}

	void RecordImageToBufferCopy(VkCommandBuffer commandBuffer, uint32_t frameIndex, VkImage colourImage, uint32_t width, uint32_t height);
	void CleanUp();

	// Accessors
	VkBuffer GetBufferForFrame(uint32_t frameIndex) const { return readbackBuffers.uniformBuffers[frameIndex]; }
	void* GetMapBufferForFrame(uint32_t frameIndex) const { return readbackBuffers.uniformBuffersMapped[frameIndex]; }
	VkDeviceSize GetBufferSize() const { return bufferSize; }
};