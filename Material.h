#pragma once
#include "Texture.h"
#include "VulkanDescriptor.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Material final {
private:
	std::unordered_map<std::string, Texture> textures;
	VulkanDescriptor vulkanDescriptor = {};
	glm::vec3 ambientColor = { 0.1f, 0.1f, 0.1f };
	glm::vec3 diffuseColor = { 0.9f, 0.7f, 0.9f };
	glm::vec3 specularColor = { 1.0f, 1.0f, 1.0f };
	glm::vec3 emissionColor = { 0.0f, 0.0f, 0.0f };
	const DescriptorVulkan* descriptorVulkan = nullptr;
	float shininess = 32.0f;
	float refractionIndex = 1.0f;
	float transparency = 1.0f;
	unsigned int illuminationModel = 2;

	void AssignTexures(const ModelMTL& mtlData);
	void AddTexture(const std::string& type, const Texture& texture);
	void CreateTexture(const CoreVulkan* coreVulkan_, const CommandPoolVulkan* commandPoolVulkan_, const VkSampler* texSampler);
	void CreateDescriptor(const CoreVulkan* coreVulkan_, const ModelBuffersVulkan* modelBuffersVulkan_, const PipelineVulkan* pipelineVulkan_, const UniformVulkan& uniformBufferObject);
public:
	Material() = default;
	~Material() = default;
	explicit Material(const ModelMTL& mtlData)
		:	textures(), 
		vulkanDescriptor(),
		ambientColor(mtlData.ambientColor),
		diffuseColor(mtlData.diffuseColor),
		specularColor(mtlData.specularColor),
		emissionColor(mtlData.emissionColor),
		descriptorVulkan(nullptr),
		shininess(mtlData.shininess),
		refractionIndex(mtlData.refractionIndex),
		transparency(mtlData.transparency),
		illuminationModel(mtlData.illuminationModel)
	{
		AssignTexures(mtlData);
	}
	Material(const Material& other) = default;
	Material& operator=(const Material& other)
	{
		if (this != &other) {
			vulkanDescriptor = other.vulkanDescriptor;
			descriptorVulkan = other.descriptorVulkan;
			textures = other.textures;
			ambientColor = other.ambientColor;
			diffuseColor = other.diffuseColor;
			specularColor = other.specularColor;
			emissionColor = other.emissionColor;
			shininess = other.shininess;
			refractionIndex = other.refractionIndex;
			transparency = other.transparency;
			illuminationModel = other.illuminationModel;
			// If you need to do any deep copy or resource management, do it here.
		}
		return *this;
	}

	void CreateMaterialResources(const CoreVulkan* coreVulkan_, 
		const CommandPoolVulkan* commandPoolVulkan_, 
		const ModelBuffersVulkan* modelBuffersVulkan_, 
		const PipelineVulkan* pipelineVulkan_, 
		const VkSampler* texSampler,
		UniformVulkan& uniformBufferObject);
	const void CleanupMaterialResources() const;

	const std::unordered_map<std::string, Texture>& GetTextures() const { return textures; }
	const DescriptorVulkan* GetDescriptorVulkan() const { return descriptorVulkan; }
	const glm::vec3& GetAmbientColor() const { return ambientColor; };
	const glm::vec3& GetDiffuseColor() const { return diffuseColor; };
	const glm::vec3& GetSpecularColor() const { return specularColor; };
	const glm::vec3& GetEmissionColor() const { return emissionColor; };
	float GetShininess() const { return shininess; };
	float GetRefractionIndex() const { return refractionIndex; };
	float GetTransparency() const { return transparency; };
};