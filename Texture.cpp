#include "Texture.h"

void Texture::CreateTexture(const CoreVulkan* coreVulkan, const CommandPoolVulkan* commandPoolVulkan, const VkSampler* texSampler, TextureDimension dimension)
{
	if (!texturePath.empty()) {
		vulkanTexture = VulkanTexture(coreVulkan, commandPoolVulkan, texturePath, texSampler, dimension);
		textureVulkan = &vulkanTexture.GetTextureVulkan();
	}
}
