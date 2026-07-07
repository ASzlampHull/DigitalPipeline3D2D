#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "WindowManager.h"
#include <algorithm>
#include <array>


class VulkanSwapChain final {
private:
	SwapChainVulkan swapChainVulkan = {};
	GLFWwindow* window = nullptr;
	const CoreVulkan* coreVulkan = nullptr;

	void CreateSwapChain();
	void CreateImageViews();

	const void ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats, VkSurfaceFormatKHR& availableFormat_) const;
	const VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
	const void ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D& actualExtent_) const;
public:
	VulkanSwapChain() = default;
	~VulkanSwapChain() = default;
	explicit VulkanSwapChain(GLFWwindow* window_, const CoreVulkan* coreVulkan_) : window(window_), coreVulkan(coreVulkan_)
	{
		InitialiseSwapChain();
	}
	VulkanSwapChain(const VulkanSwapChain& other) = default;
	VulkanSwapChain& operator=(const VulkanSwapChain& rhs)
	{
		if (this != &rhs) {
			this->swapChainVulkan = rhs.swapChainVulkan;
			this->window = rhs.window;
			this->coreVulkan = rhs.coreVulkan;
		}
		return *this;
	}

	const SwapChainVulkan& GetSwapChainVulkan() const { return swapChainVulkan; };

	void InitialiseSwapChain();
	void Cleanup();
};