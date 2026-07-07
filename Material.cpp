#include "Material.h"

void Material::AssignTexures(const ModelMTL& mtlData)
{
	AddTexture("diffuse", Texture(mtlData.texturePath));
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
		texture.second.CreateTexture(coreVulkan_, commandPoolVulkan_, texSampler);
	}
}

void Material::CreateDescriptor(const CoreVulkan* coreVulkan_, const ModelBuffersVulkan* modelBuffersVulkan_, const PipelineVulkan* pipelineVulkan_, const UniformVulkan& uniformBufferObject)
{
	//TODO: Handle multiple texture types in descriptor 1 
	//binding 0->uniform buffer(material params / transform)
	//binding 1->diffuse(albedo) sampler
	//binding 2->normal map sampler
	//binding 3->bump / height map sampler

	vulkanDescriptor = VulkanDescriptor(coreVulkan_, modelBuffersVulkan_, pipelineVulkan_, textures.at("diffuse").GetTextureVulkan(), uniformBufferObject);
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
