#include "VulkanFramebuffers.h"

void VulkanFramebuffers::InitialiseFramebuffers()
{
	CreateCommandPool();
	CreateDepthResources();
	CreateFramebuffers();
}

void VulkanFramebuffers::CreateCommandPool()
{
    QueueFamilyIndices queueFamilyIndices;
    VulkQuery::FindQueueFamilies(coreVulkan->physicalDevice, coreVulkan->surface, queueFamilyIndices);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(coreVulkan->device, &poolInfo, nullptr, &commandPoolVulkan.commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics command pool!");
    }
}

void VulkanFramebuffers::CreateDepthResources()
{
    const VkFormat depthFormat = VulkQuery::FindDepthFormat(coreVulkan->physicalDevice);

    VulkCreate::CreateImage(swapChainVulkan->swapChainExtent.width, swapChainVulkan->swapChainExtent.height, depthFormat,
        VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        depthVulkan.depthImage, depthVulkan.depthImageMemory, coreVulkan->device, coreVulkan->physicalDevice);

    depthVulkan.depthImageView = VulkCreate::CreateImageView(coreVulkan->device, depthVulkan.depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void VulkanFramebuffers::CreateFramebuffers()
{
    swapChainVulkan->swapChainFramebuffers.resize(swapChainVulkan->swapChainImageViews.size());

    for (size_t i = 0; i < swapChainVulkan->swapChainImageViews.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            swapChainVulkan->swapChainImageViews[i],
            depthVulkan.depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = pipelineVulkan->renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = swapChainVulkan->swapChainExtent.width;
        framebufferInfo.height = swapChainVulkan->swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(coreVulkan->device, &framebufferInfo, nullptr, &swapChainVulkan->swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

const void VulkanFramebuffers::CleanupDepth() const
{
    vkDestroyImageView(coreVulkan->device, depthVulkan.depthImageView, nullptr);
    vkDestroyImage(coreVulkan->device, depthVulkan.depthImage, nullptr);
    vkFreeMemory(coreVulkan->device, depthVulkan.depthImageMemory, nullptr);
}
