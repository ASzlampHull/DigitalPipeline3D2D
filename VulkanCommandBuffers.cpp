#include "VulkanCommandBuffers.h"

void VulkanCommandBuffers::CreateCommandBuffers()
{
    commandBuffersVulkan.commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPoolVulkan->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffersVulkan.commandBuffers.size());

    if (vkAllocateCommandBuffers(coreVulkan->device, &allocInfo, commandBuffersVulkan.commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}