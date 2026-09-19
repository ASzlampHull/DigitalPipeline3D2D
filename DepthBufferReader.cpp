#include "DepthBufferReader.h"

void DepthBufferReader::Initialise()
{
	frameCount = 2;
	VkDeviceSize size = sizeof(float) * width * height;

	depthBuffers.uniformBuffers.resize(frameCount);
	depthBuffers.uniformBuffersMapped.resize(frameCount);
	depthBuffers.uniformBuffersMemory.resize(frameCount);

	for (uint32_t i = 0; i < frameCount; ++i)
	{
		VulkCreate::CreateBuffer(size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			depthBuffers.uniformBuffers[i], depthBuffers.uniformBuffersMemory[i], coreVulkan->device, coreVulkan->physicalDevice);
		vkMapMemory(coreVulkan->device, depthBuffers.uniformBuffersMemory[i], 0, size, 0, &depthBuffers.uniformBuffersMapped[i]);
	}
}

// Copies the depth buffer image to a host-visible buffer for reading
void DepthBufferReader::RecordDepthCopy(VkCommandBuffer commandBuffer, uint32_t frameIndex, VkImage depthImage)
{
    // Transition depth image to TRANSFER_SRC using Synchronization2
    VkImageMemoryBarrier2 barrier2{};
    barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier2.pNext = nullptr;
    barrier2.srcStageMask = VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT;
    barrier2.srcAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    barrier2.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier2.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
    barrier2.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    barrier2.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier2.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier2.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier2.image = depthImage;
    barrier2.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;
    depInfo.dependencyFlags = 0;
    depInfo.memoryBarrierCount = 0;
    depInfo.pMemoryBarriers = nullptr;
    depInfo.bufferMemoryBarrierCount = 0;
    depInfo.pBufferMemoryBarriers = nullptr;
    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &barrier2;

    vkCmdPipelineBarrier2(commandBuffer, &depInfo);

    VkBufferImageCopy region{};
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = { width, height, 1 };

    vkCmdCopyImageToBuffer(commandBuffer,
        depthImage,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        depthBuffers.uniformBuffers[frameIndex],
        1, &region);
}

void DepthBufferReader::CleanUp()
{
	for (uint32_t i = 0; i < frameCount; ++i)
	{
		vkUnmapMemory(coreVulkan->device, depthBuffers.uniformBuffersMemory[i]);
		vkDestroyBuffer(coreVulkan->device, depthBuffers.uniformBuffers[i], nullptr);
		vkFreeMemory(coreVulkan->device, depthBuffers.uniformBuffersMemory[i], nullptr);
	}
}
