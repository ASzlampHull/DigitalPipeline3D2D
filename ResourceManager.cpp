#include "ResourceManager.h"

void ResourceManager::CreateModels() {
	models.clear();
	for (const auto& modelData : modelLoader.GetLoadedModels()) {
		const Model model(modelData);
		models[modelData.name] = model;
	}
}

void ResourceManager::CreateParticles()
{
	fireParticles = Particles();
}

void ResourceManager::CreateMainTextureSampler(const CoreVulkan* coreVulkan)
{
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

	if (vkCreateSampler(coreVulkan->device, &samplerInfo, nullptr, &textureSamplers["Main"]) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void ResourceManager::CreateTextures(const CoreVulkan* coreVulkan, 
	const CommandPoolVulkan* commandPoolVulkan,
	const PipelineVulkan* pipelineVulkan,
	UniformVulkan& uniformBufferObject)
{
	CreateMainTextureSampler(coreVulkan);
	for (auto& pair : models) {
		auto& model = pair.second;
		model.CreateMaterialResources(coreVulkan, commandPoolVulkan, model.GetModelBuffersVulkan(), pipelineVulkan, &textureSamplers["Main"], uniformBufferObject);
	}
}

void ResourceManager::CleanupTextures(const CoreVulkan* coreVulkan)
{
	for (auto& samplerPair : textureSamplers) {
		vkDestroySampler(coreVulkan->device, samplerPair.second, nullptr);
	}

	for (auto& pair : models) {
		auto& model = pair.second;
		model.CleanupMaterial();
	}
}

void ResourceManager::CreateVertexIndexBuffers(const CoreVulkan* coreVulkan, const CommandPoolVulkan* commandPoolVulkan)
{
	for (auto& pair : models) {
		auto& model = pair.second;
		model.CreateVertexIndexBuffers(coreVulkan, commandPoolVulkan);
	}
}

void ResourceManager::CleanupBuffersVI()
{
	for (const auto& pair : models) {
		const auto& model = pair.second;
		model.CleanupBuffersVI();
	}
}

const VkDescriptorPool ResourceManager::GetMainDescriptorPool() const
{
	const auto& pair = models.begin();
	const auto& model = pair->second;
	const auto& material = model.GetMaterial();
	if (material.GetDescriptorVulkan() != nullptr) {
		return material.GetDescriptorVulkan()->descriptorPool;
	}

	__debugbreak(); // No models / materials available to get descriptor pool from
	return VkDescriptorPool();
}
