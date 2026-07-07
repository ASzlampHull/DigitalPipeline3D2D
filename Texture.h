#pragma once
#include "ModelParserDataTypes.h"
#include "VulkanTexture.h"

class Texture final {
private:
	std::string texturePath = "";
	VulkanTexture vulkanTexture = {};
	const TextureVulkan* textureVulkan = nullptr;

public:
	Texture() = default;
	~Texture() = default;
	explicit Texture(const std::string& texturePath_) : texturePath(texturePath_) {}
	Texture(const Texture& other) = default;
    Texture& operator=(const Texture& other)
    {
        if (this != &other) {
            texturePath = other.texturePath;
            vulkanTexture = other.vulkanTexture;
            textureVulkan = other.textureVulkan;
        }
        return *this;
    }

	void CreateTexture(const CoreVulkan* coreVulkan, const CommandPoolVulkan* commandPoolVulkan, const VkSampler* texSampler);
	const void Cleanup() const { vulkanTexture.Cleanup(); }

	// Add methods for loading and managing textures
	const std::string& GetTexturePath() const { return texturePath; }
	const TextureVulkan* GetTextureVulkan() const { return textureVulkan; }
};
