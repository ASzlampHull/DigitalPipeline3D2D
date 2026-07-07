#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <stdexcept>

#pragma region Vulkan Check Structures

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() const {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

#pragma endregion

#pragma region Configuration

extern const std::vector<const char*> validationLayers;
extern const std::vector<const char*> deviceExtensions;

#pragma endregion

#pragma region Debug Messenger Functions

namespace VulkDebugMessenger {
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
}

#pragma endregion

#pragma region Query Functions

namespace VulkQuery {
    void FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface, QueueFamilyIndices& indices_);
    void QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface, SwapChainSupportDetails& details_);
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features, VkPhysicalDevice physicalDevice);
    VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);
}

#pragma endregion

#pragma region Create Generic Vulkan Objects

namespace VulkCreate {
    VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory, VkDevice device, VkPhysicalDevice physicalDevice);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory, VkDevice device, VkPhysicalDevice physicalDevice);
}

#pragma endregion

#pragma region Command Buffer Helpers

namespace VulkCommands {
    VkCommandBuffer BeginSingleTimeCommands(VkCommandPool commandPool, VkDevice device);
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool commandPool, VkQueue graphicsQueue, VkDevice device);
}

#pragma endregion