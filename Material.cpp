#include "Material.h"

void Material::AssignTexures(const ModelMTL& mtlData)
{
	AddTexture("diffuse", Texture(mtlData.texturePath));
	AddTexture("cel_shading", Texture("textures/CelShading1DTexture.png"));
	//AddTexture("normal", Texture()); //TODO: Implement normal texture loading
	//AddTexture("bump", Texture());
}

void Material::AddTexture(const std::string& type, const Texture& texture)
{
	textures[type] = texture;
}

void Material::CreateTexture(const CoreVulkan* coreVulkan_, const CommandPoolVulkan* commandPoolVulkan_, const VkSampler* texSampler)
{
	for (auto& texture : textures) {
		if (texture.first == "diffuse")
			texture.second.CreateTexture(coreVulkan_, commandPoolVulkan_, texSampler, TextureDimension::Texture2D);
		else if (texture.first == "cel_shading")
			texture.second.CreateTexture(coreVulkan_, commandPoolVulkan_, texSampler, TextureDimension::Texture1D);
		else
			texture.second.CreateTexture(coreVulkan_, commandPoolVulkan_, texSampler, TextureDimension::Texture2D);
	}
}

void Material::CreateDescriptor(const CoreVulkan* coreVulkan_, const ModelBuffersVulkan* modelBuffersVulkan_, const PipelineVulkan* pipelineVulkan_, const UniformVulkan& uniformBufferObject)
{
	vulkanDescriptor = VulkanDescriptor(coreVulkan_, modelBuffersVulkan_, pipelineVulkan_, textures.at("diffuse").GetTextureVulkan(), textures.at("cel_shading").GetTextureVulkan(), uniformBufferObject);
	descriptorVulkan = &vulkanDescriptor.GetDescriptorVulkan();
}

void Material::CreateMaterialResources(const CoreVulkan* coreVulkan_, 
	const CommandPoolVulkan* commandPoolVulkan_, 
	const ModelBuffersVulkan* modelBuffersVulkan_, 
	const PipelineVulkan* pipelineVulkan_, 
	const VkSampler* texSampler,
	UniformVulkan& uniformBufferObject)
{
	CreateTexture(coreVulkan_, commandPoolVulkan_, texSampler);
	CreateDescriptor(coreVulkan_, modelBuffersVulkan_, pipelineVulkan_, uniformBufferObject);
}

const void Material::CleanupMaterialResources() const
{
	for (const auto& texture : textures) {
		texture.second.Cleanup();
	}
	vulkanDescriptor.Cleanup();
}
