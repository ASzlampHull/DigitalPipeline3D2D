#include "ReadbackImageTarget.h"

void ReadbackImageTarget::Initialise()
{
    CreateImage(textureVulkanColour,
        config.width, config.height, config.colorFormat,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

    CreateImage(textureVulkanDepth,
        config.width, config.height, config.depthFormat,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
}

void ReadbackImageTarget::CreateImage(TextureVulkan& texVulkan, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage)
{
	VulkCreate::CreateImage(width, height, format, 
        VK_IMAGE_TILING_OPTIMAL, 
        usage,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		texVulkan.textureImage, texVulkan.textureImageMemory,
		coreVulkan->device, coreVulkan->physicalDevice, 
        VK_IMAGE_TYPE_2D);
	texVulkan.textureImageView = VulkCreate::CreateImageView(coreVulkan->device, texVulkan.textureImage, 
        format, usage,
		VK_IMAGE_VIEW_TYPE_2D);
}

// Transition the images to the appropriate layouts for rendering (color attachment and depth attachment)
void ReadbackImageTarget::TransitionForRendering(VkCommandBuffer commandBuffer)
{
    VkImageMemoryBarrier2 barriers[2]{};

    barriers[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barriers[0].pNext = nullptr;
    barriers[0].srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    barriers[0].srcAccessMask = 0;
    barriers[0].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    barriers[0].dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    barriers[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barriers[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barriers[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[0].image = textureVulkanColour.textureImage;
    barriers[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barriers[0].subresourceRange.levelCount = 1;
    barriers[0].subresourceRange.layerCount = 1;

    barriers[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barriers[1].pNext = nullptr;
    barriers[1].srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
    barriers[1].srcAccessMask = 0;
    barriers[1].dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT;
    barriers[1].dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    barriers[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barriers[1].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    barriers[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barriers[1].image = textureVulkanDepth.textureImage;
    barriers[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    barriers[1].subresourceRange.levelCount = 1;
    barriers[1].subresourceRange.layerCount = 1;

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;
    depInfo.dependencyFlags = 0;
    depInfo.memoryBarrierCount = 0;
    depInfo.pMemoryBarriers = nullptr;
    depInfo.bufferMemoryBarrierCount = 0;
    depInfo.pBufferMemoryBarriers = nullptr;
    depInfo.imageMemoryBarrierCount = 2;
    depInfo.pImageMemoryBarriers = barriers;

    vkCmdPipelineBarrier2(commandBuffer, &depInfo);
}

// Transition the color image to transfer source layout for readback
void ReadbackImageTarget::TransitionForReadback(VkCommandBuffer commandBuffer)
{
    VkImageMemoryBarrier2 barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    barrier.pNext = nullptr;
    barrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
    barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
    barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = textureVulkanColour.textureImage;
    barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

    VkDependencyInfo depInfo{};
    depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfo.pNext = nullptr;
    depInfo.dependencyFlags = 0;
    depInfo.memoryBarrierCount = 0;
    depInfo.pMemoryBarriers = nullptr;
    depInfo.bufferMemoryBarrierCount = 0;
    depInfo.pBufferMemoryBarriers = nullptr;
    depInfo.imageMemoryBarrierCount = 1;
    depInfo.pImageMemoryBarriers = &barrier;

    vkCmdPipelineBarrier2(commandBuffer, &depInfo);
}

void ReadbackImageTarget::CleanUp()
{
	vkDestroyImage(coreVulkan->device, textureVulkanColour.textureImage, nullptr);
	vkDestroyImageView(coreVulkan->device, textureVulkanColour.textureImageView, nullptr);
	vkFreeMemory(coreVulkan->device, textureVulkanColour.textureImageMemory, nullptr);

	vkDestroyImage(coreVulkan->device, textureVulkanDepth.textureImage, nullptr);
	vkDestroyImageView(coreVulkan->device, textureVulkanDepth.textureImageView, nullptr);
	vkFreeMemory(coreVulkan->device, textureVulkanDepth.textureImageMemory, nullptr);
}
