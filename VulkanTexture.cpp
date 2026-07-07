#include "VulkanTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void VulkanTexture::InitialiseTexture(const std::string& texturePath_)
{
	const char* const texturePath = texturePath_.c_str();
	CreateTextureImage(texturePath);
	CreateTextureImageView();
}

void VulkanTexture::CreateTextureImage(const char* texturePath)
{
    int texWidth, texHeight, texChannels;
    const stbi_uc* pixels = stbi_load(texturePath, &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    const VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VulkCreate::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, coreVulkan->device, coreVulkan->physicalDevice);

    void* data;
    vkMapMemory(coreVulkan->device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(coreVulkan->device, stagingBufferMemory);

    //stbi_image_free(pixels);

    VulkCreate::CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureVulkan.textureImage, textureVulkan.textureImageMemory, coreVulkan->device, coreVulkan->physicalDevice);

    TransitionImageLayout(textureVulkan.textureImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(stagingBuffer, textureVulkan.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    TransitionImageLayout(textureVulkan.textureImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(coreVulkan->device, stagingBuffer, nullptr);
    vkFreeMemory(coreVulkan->device, stagingBufferMemory, nullptr);
}

void VulkanTexture::CreateTextureImageView()
{
    textureVulkan.textureImageView = VulkCreate::CreateImageView(coreVulkan->device, textureVulkan.textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

const void VulkanTexture::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const
{
    const VkCommandBuffer commandBuffer = VulkCommands::BeginSingleTimeCommands(commandPoolVulkan->commandPool, coreVulkan->device);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    VulkCommands::EndSingleTimeCommands(commandBuffer, commandPoolVulkan->commandPool, coreVulkan->graphicsQueue, coreVulkan->device);
}

const void VulkanTexture::TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) const
{
    const VkCommandBuffer commandBuffer = VulkCommands::BeginSingleTimeCommands(commandPoolVulkan->commandPool, coreVulkan->device);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    VulkCommands::EndSingleTimeCommands(commandBuffer, commandPoolVulkan->commandPool, coreVulkan->graphicsQueue, coreVulkan->device);
}

const void VulkanTexture::Cleanup() const
{
    vkDestroyImageView(coreVulkan->device, textureVulkan.textureImageView, nullptr);

    vkDestroyImage(coreVulkan->device, textureVulkan.textureImage, nullptr);
    vkFreeMemory(coreVulkan->device, textureVulkan.textureImageMemory, nullptr);
}
