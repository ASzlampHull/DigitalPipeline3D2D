#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include <stdexcept>
#include <string>
#include <iostream>


class VulkanTexture final {
private:
	TextureVulkan textureVulkan = {};
	const CoreVulkan* coreVulkan = nullptr;
	const CommandPoolVulkan* commandPoolVulkan = nullptr;

	void InitialiseTexture(const std::string& texturePath);
	void CreateTextureImage(const char* texturePath);
	void CreateTextureImageView();

	const void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) const;
	const void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout) const;
public:
	VulkanTexture() = default;
	~VulkanTexture() = default;
	explicit VulkanTexture(const CoreVulkan* coreVulkan_, const CommandPoolVulkan* commandPoolVulkan_, const std::string& texturePath, const VkSampler* texSampler)
		: coreVulkan(coreVulkan_),
		commandPoolVulkan(commandPoolVulkan_)
	{
		textureVulkan.textureSampler = texSampler;
		InitialiseTexture(texturePath);
	}
	VulkanTexture(const VulkanTexture& other) = default;
	VulkanTexture& operator=(const VulkanTexture& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->commandPoolVulkan = rhs.commandPoolVulkan;
			this->textureVulkan = rhs.textureVulkan;
		}
		return *this;
	}

	const TextureVulkan& GetTextureVulkan() const { return textureVulkan; };
	const void Cleanup() const;
};