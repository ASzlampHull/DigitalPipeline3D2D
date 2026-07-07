#include "VulkanSwapChain.h"

void VulkanSwapChain::InitialiseSwapChain()
{
	CreateSwapChain();
	CreateImageViews();
}

void VulkanSwapChain::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport;
	VulkQuery::QuerySwapChainSupport(coreVulkan->physicalDevice, coreVulkan->surface, swapChainSupport);
	
	VkSurfaceFormatKHR surfaceFormat;
	ChooseSwapSurfaceFormat(swapChainSupport.formats, surfaceFormat);
	const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent;
	ChooseSwapExtent(swapChainSupport.capabilities, extent);

	swapChainVulkan.imageCountMin = swapChainSupport.capabilities.minImageCount;
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = coreVulkan->surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices;
	VulkQuery::FindQueueFamilies(coreVulkan->physicalDevice, coreVulkan->surface, indices);

	if (indices.graphicsFamily != indices.presentFamily) {
		std::array<uint32_t, 2> queueFamilyIndices = { indices.graphicsFamily.value(), indices.presentFamily.value() };
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
		createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
	}
	
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	
	if (vkCreateSwapchainKHR(coreVulkan->device, &createInfo, nullptr, &swapChainVulkan.swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}
	
	vkGetSwapchainImagesKHR(coreVulkan->device, swapChainVulkan.swapChain, &imageCount, nullptr);
	swapChainVulkan.swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(coreVulkan->device, swapChainVulkan.swapChain, &imageCount, swapChainVulkan.swapChainImages.data());
	
	swapChainVulkan.swapChainImageFormat = surfaceFormat.format;
	swapChainVulkan.swapChainExtent = extent;
	swapChainVulkan.imageCount = imageCount;
}

void VulkanSwapChain::CreateImageViews()
{
	swapChainVulkan.swapChainImageViews.resize(swapChainVulkan.swapChainImages.size());

	for (uint32_t i = 0; i < swapChainVulkan.swapChainImages.size(); i++) {
		swapChainVulkan.swapChainImageViews[i] = VulkCreate::CreateImageView(coreVulkan->device, swapChainVulkan.swapChainImages[i], swapChainVulkan.swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

const void VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats, VkSurfaceFormatKHR& availableFormat_) const
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			availableFormat_ = availableFormat;
		}
	}

	availableFormat_ = availableFormats[0];
}

const VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

const void VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, VkExtent2D& actualExtent_) const
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		actualExtent_ = capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		actualExtent_ = actualExtent;
	}
}

void VulkanSwapChain::Cleanup()
{
	for (const auto framebuffer : swapChainVulkan.swapChainFramebuffers) {
		vkDestroyFramebuffer(coreVulkan->device, framebuffer, nullptr);
	}

	for (const auto imageView : swapChainVulkan.swapChainImageViews) {
		vkDestroyImageView(coreVulkan->device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(coreVulkan->device, swapChainVulkan.swapChain, nullptr);
}
