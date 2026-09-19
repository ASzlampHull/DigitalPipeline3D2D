#pragma once
#include "VulkanDataTypes.h"
#include "FeatureReadbackTypes.h"
#include "VulkanExternals.h"

// This class manages a Vulkan framebuffer with color and depth attachments for rendering and readback of feature data
class ReadbackImageTarget
{
private:
    const CoreVulkan* coreVulkan = nullptr;
	TextureVulkan textureVulkanColour = {};
	TextureVulkan textureVulkanDepth = {};
    ReadbackTargetConfig config{};

	void Initialise();
	void CreateImage(TextureVulkan& texVulkan, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usage);

public:
	ReadbackImageTarget() = default;
	~ReadbackImageTarget() = default;
	ReadbackImageTarget(const CoreVulkan* coreVulkan_, const ReadbackTargetConfig& config_) : coreVulkan(coreVulkan_), config(config_)
	{
		Initialise();
	}
	ReadbackImageTarget(const ReadbackImageTarget& other) = default;
	ReadbackImageTarget& operator=(const ReadbackImageTarget& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->textureVulkanColour = rhs.textureVulkanColour;
			this->textureVulkanDepth = rhs.textureVulkanDepth;
			this->config = rhs.config;
		}
		return *this;
	}

	void TransitionForRendering(VkCommandBuffer commandBuffer);
	void TransitionForReadback(VkCommandBuffer commandBuffer);

	// Accessors
	const TextureVulkan& GetColourTexture() const { return textureVulkanColour; }
	const TextureVulkan& GetDepthTexture() const { return textureVulkanDepth; }
	const ReadbackTargetConfig& GetConfig() const { return config; }

	void CleanUp();
};