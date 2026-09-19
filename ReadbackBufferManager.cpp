#include "ReadbackBufferManager.h"

void ReadbackBufferManager::Initialise(VkDeviceSize bytesPerPixel)
{
	bufferSize = static_cast<VkDeviceSize>(width) * static_cast<VkDeviceSize>(height) * bytesPerPixel;
	readbackBuffers.uniformBuffers.resize(frameCount);
	readbackBuffers.uniformBuffersMapped.resize(frameCount);
	readbackBuffers.uniformBuffersMemory.resize(frameCount);

	for (uint32_t i = 0; i < frameCount; ++i)
	{
		VulkCreate::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			readbackBuffers.uniformBuffers[i], readbackBuffers.uniformBuffersMemory[i], coreVulkan->device, coreVulkan->physicalDevice);
		vkMapMemory(coreVulkan->device, readbackBuffers.uniformBuffersMemory[i], 0, bufferSize, 0, &readbackBuffers.uniformBuffersMapped[i]);
	}
}

// Record the copy command (image -> staging buffer)
void ReadbackBufferManager::RecordImageToBufferCopy(VkCommandBuffer commandBuffer, uint32_t frameIndex, VkImage colourImage, uint32_t width, uint32_t height)
{
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = { width, height, 1 };

    vkCmdCopyImageToBuffer(commandBuffer,
        colourImage,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        readbackBuffers.uniformBuffers[frameIndex],
        1, &region);
}

void ReadbackBufferManager::CleanUp()
{
	for (uint32_t i = 0; i < frameCount; ++i)
	{
		vkUnmapMemory(coreVulkan->device, readbackBuffers.uniformBuffersMemory[i]);
		vkDestroyBuffer(coreVulkan->device, readbackBuffers.uniformBuffers[i], nullptr);
		vkFreeMemory(coreVulkan->device, readbackBuffers.uniformBuffersMemory[i], nullptr);
	}
}
