#include "Texture.h"

void Texture::CreateTexture(const CoreVulkan* coreVulkan, const CommandPoolVulkan* commandPoolVulkan, const VkSampler* texSampler)
{
	if (!texturePath.empty()) {
		vulkanTexture = VulkanTexture(coreVulkan, commandPoolVulkan, texturePath, texSampler);
		textureVulkan = &vulkanTexture.GetTextureVulkan();
	}
}
