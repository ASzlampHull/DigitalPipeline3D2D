#include "PostProcessingLines.h"

void PostProcessingLines::CreateScreenTextures()
{
	int width = textureInputScreen->swapChainExtent.width;
	int height = textureInputScreen->swapChainExtent.height;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(coreVulkan->physicalDevice, &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	if (vkCreateSampler(coreVulkan->device, &samplerInfo, nullptr, &textureSamplerInputScreen) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}

	//VulkCreate::CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureInputScreen.textureImage, textureInputScreen.textureImageMemory, coreVulkan->device, coreVulkan->physicalDevice, VK_IMAGE_TYPE_2D);
	//textureInputScreen.textureImageView = VulkCreate::CreateImageView(coreVulkan->device, textureInputScreen.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D);
	//textureInputScreen.textureSampler = &textureSamplerInputScreen;

	VulkCreate::CreateImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureOutputScreen.textureImage, textureOutputScreen.textureImageMemory, coreVulkan->device, coreVulkan->physicalDevice, VK_IMAGE_TYPE_2D);
	textureOutputScreen.textureImageView = VulkCreate::CreateImageView(coreVulkan->device, textureOutputScreen.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D);
	textureOutputScreen.textureSampler = VK_NULL_HANDLE;
}

void PostProcessingLines::CreateDescriptorPool()
{
	const uint32_t maxSets = 1000;
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[0].descriptorCount = maxSets;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	poolSizes[1].descriptorCount = maxSets;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets = maxSets;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();

	if (vkCreateDescriptorPool(coreVulkan->device, &poolInfo, nullptr, &descriptorVulkan.descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute descriptor pool!");
	}
}

void PostProcessingLines::CreateDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> computeLayouts(MAX_FRAMES_IN_FLIGHT, pipelineVulkan->descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocSetInfo{};
	allocSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocSetInfo.descriptorPool = descriptorVulkan.descriptorPool;
	allocSetInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocSetInfo.pSetLayouts = computeLayouts.data();

	descriptorVulkan.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(coreVulkan->device, &allocSetInfo, descriptorVulkan.descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate compute descriptor sets!");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		VkDescriptorImageInfo inputImageInfo{};
		inputImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		inputImageInfo.imageView = textureInputScreen->swapChainImageViews[i];
		inputImageInfo.sampler = textureSamplerInputScreen;

		VkDescriptorImageInfo outputImageInfo{};
		outputImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		outputImageInfo.imageView = textureOutputScreen.textureImageView;
		outputImageInfo.sampler = VK_NULL_HANDLE;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		// Write descriptor for input combined image sampler
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorVulkan.descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &inputImageInfo;

		// Write descriptor for output storage image
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorVulkan.descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &outputImageInfo;

		vkUpdateDescriptorSets(coreVulkan->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void PostProcessingLines::UpdateInputDescriptorForCurrentImage(uint32_t currentFrame, uint32_t imageIndex)
{
	VkDescriptorImageInfo inputImageInfo{};
	inputImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	inputImageInfo.imageView = textureInputScreen->swapChainImageViews[imageIndex];
	inputImageInfo.sampler = textureSamplerInputScreen;

	VkWriteDescriptorSet inputWrite{};
	inputWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	inputWrite.dstSet = descriptorVulkan.descriptorSets[currentFrame];
	inputWrite.dstBinding = 0;
	inputWrite.dstArrayElement = 0;
	inputWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	inputWrite.descriptorCount = 1;
	inputWrite.pImageInfo = &inputImageInfo;

	vkUpdateDescriptorSets(coreVulkan->device, 1, &inputWrite, 0, nullptr);
}

void PostProcessingLines::CleanUp()
{
	vkDestroyImage(coreVulkan->device, textureOutputScreen.textureImage, nullptr);
	vkDestroyImageView(coreVulkan->device, textureOutputScreen.textureImageView, nullptr);
	vkFreeMemory(coreVulkan->device, textureOutputScreen.textureImageMemory, nullptr);

	vkDestroyDescriptorPool(coreVulkan->device, descriptorVulkan.descriptorPool, nullptr);
}