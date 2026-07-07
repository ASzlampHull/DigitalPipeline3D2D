#include "VulkanFrameSync.h"

void VulkanFrameSync::CreateSyncObjects()
{
    frameSyncVulkan.imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    frameSyncVulkan.renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    frameSyncVulkan.inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(coreVulkan->device, &semaphoreInfo, nullptr, &frameSyncVulkan.imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(coreVulkan->device, &semaphoreInfo, nullptr, &frameSyncVulkan.renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(coreVulkan->device, &fenceInfo, nullptr, &frameSyncVulkan.inFlightFences[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

void VulkanFrameSync::Cleanup()
{
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(coreVulkan->device, frameSyncVulkan.renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(coreVulkan->device, frameSyncVulkan.imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(coreVulkan->device, frameSyncVulkan.inFlightFences[i], nullptr);
    }
}
