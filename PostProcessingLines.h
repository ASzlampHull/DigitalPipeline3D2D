#pragma once
#include "VulkanExternals.h"
#include "VulkanDataTypes.h"
#include <array>


class PostProcessingLines
{
private:
	// Constants
	const uint32_t numElements = 10000;

	// Vulkan handlers
	DescriptorVulkan descriptorVulkan = {};
	const CoreVulkan* coreVulkan = nullptr;
	const PipelineVulkan* pipelineVulkan = nullptr;

	// Vulkan handlers for the input screen texture
	const SwapChainVulkan* textureInputScreen;
	VkSampler textureSamplerInputScreen = VK_NULL_HANDLE;

	// Vulkan handlers for the output screen texture
	TextureVulkan textureOutputScreen;

	void CreateScreenTextures();
	void CreateDescriptorPool();
	void CreateDescriptorSet();

public:
	PostProcessingLines() = default;
	~PostProcessingLines() = default;
	PostProcessingLines(const CoreVulkan* coreVulkan_, const PipelineVulkan* pipelineVulkan_, const SwapChainVulkan* textureInputScreen_)
		: coreVulkan(coreVulkan_),
		pipelineVulkan(pipelineVulkan_),
		textureInputScreen(textureInputScreen_)
	{

		CreateScreenTextures();
		CreateDescriptorPool();
		CreateDescriptorSet();
	}
	PostProcessingLines(const PostProcessingLines& other) = default;
	PostProcessingLines& operator=(const PostProcessingLines& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->descriptorVulkan = rhs.descriptorVulkan;
			this->pipelineVulkan = rhs.pipelineVulkan;
			this->textureInputScreen = rhs.textureInputScreen;
			this->textureSamplerInputScreen = rhs.textureSamplerInputScreen;
			this->textureOutputScreen = rhs.textureOutputScreen;
		}
		return *this;
	}

	void UpdateInputDescriptorForCurrentImage(uint32_t currentFrame, uint32_t imageIndex);

	const uint32_t GetNumElements() const { return numElements; }
	const DescriptorVulkan& GetDescriptorVulkan() const { return descriptorVulkan; }
	const TextureVulkan& GetOutputScreenTexture() const { return textureOutputScreen; }

	void CleanUp();
};