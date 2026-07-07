#include "Model.h"

void Model::UpdatePushConstants(VkCommandBuffer commandBuffer, const PipelineVulkan* pipelineVulkan) const
{
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, transformations.position);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(transformations.rotation.x), glm::vec3(-1.0f, 0.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(transformations.rotation.y), glm::vec3(0.0f, -1.0f, 0.0f));
	modelMatrix = glm::rotate(modelMatrix, glm::radians(transformations.rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));
	modelMatrix = glm::scale(modelMatrix, transformations.scale);
	ModelPushConstants pushConstants{};
	pushConstants.modelMatrix = modelMatrix;
	pushConstants.ambientColor = material.GetAmbientColor();
	pushConstants.diffuseColor = material.GetDiffuseColor();
	pushConstants.specularColor = material.GetSpecularColor();
	pushConstants.emissionColor = material.GetEmissionColor();
	pushConstants.shininess = material.GetShininess();
	pushConstants.refractionIndex = material.GetRefractionIndex();
	pushConstants.transparency = material.GetTransparency();

	// HARD-CODED FOR TIME //TODO: Remove hard-coding
	if (name.find("ehr_coffee") != std::string::npos)
		pushConstants.isVertexShaded = true;
	else
		pushConstants.isVertexShaded = false;

	if (name.find("ground") != std::string::npos)
		pushConstants.isBumpHeight = true;
	else
		pushConstants.isBumpHeight = false;

	vkCmdPushConstants(commandBuffer, 
		pipelineVulkan->pipelineLayout,
		VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
		0, 
		sizeof(ModelPushConstants), 
		&pushConstants
		);
}

void Model::CreateVertexIndexBuffers(const CoreVulkan* coreVulkan, const CommandPoolVulkan* commandPoolVulkan)
{
	vulkanBuffers = VulkanBuffers(coreVulkan, commandPoolVulkan, mesh.GetMeshObject(), mesh.GetVertexIndices());
	modelBuffersVulkan = &vulkanBuffers.GetModelBuffersVulkan();
}

void Model::CreateMaterialResources(const CoreVulkan* coreVulkan_, const CommandPoolVulkan* commandPoolVulkan_, const ModelBuffersVulkan* modelBuffersVulkan_, const PipelineVulkan* pipelineVulkan_, const VkSampler* texSampler, UniformVulkan& uniformBufferObject)
{
	material.CreateMaterialResources(coreVulkan_, commandPoolVulkan_, modelBuffersVulkan_, pipelineVulkan_, texSampler, uniformBufferObject);
}
