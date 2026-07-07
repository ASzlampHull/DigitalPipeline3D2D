#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "WindowManager.h"
#include <stdexcept>
#include <string>
#include <iostream>
#include <set>


class VulkanCore final {
private:
	CoreVulkan coreVulkan = {};
	GLFWwindow* window = nullptr;

	void InitialiseVulkanCore();
	void CreateInstance();
	void SetupDebugMessenger();
	void CreateSurface();
	void PickPhysicalDevice();
	void CreateLogicalDevice();

	const void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) const;
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	const bool IsDeviceSuitable(VkPhysicalDevice device) const;
	const void GetRequiredExtensions(std::vector<const char*>& extensions_) const;
	const bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;
	const bool CheckValidationLayerSupport() const;
public:

	VulkanCore() = default;
	explicit VulkanCore(GLFWwindow* glfwWindow) : window(glfwWindow)
	{ 
		InitialiseVulkanCore();
	}
	VulkanCore(const VulkanCore& other) = default;
	VulkanCore& operator=(const VulkanCore& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->window = rhs.window;
		}
		return *this;
	}

	const CoreVulkan& GetCoreVulkan() const { return coreVulkan; };
	const void Cleanup() const;
};