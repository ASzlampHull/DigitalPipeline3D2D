#include "Particles.h"

void Particles::InitialiseParticles()
{
	CreateParticleMesh();
	SetTransformations();
}

void Particles::CreateParticleMesh()
{
    for (uint32_t i = 0; i < MAX_PARTICLES; ++i)
    {
        float particleID = static_cast<float>(i) / static_cast<float>(MAX_PARTICLES);

        particleMeshObject.push_back({ {-1.0f, -1.0f, particleID}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} });
        particleMeshObject.push_back({ { 1.0f, -1.0f, particleID}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} });
        particleMeshObject.push_back({ { 1.0f,  1.0f, particleID}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} });
        particleMeshObject.push_back({ {-1.0f,  1.0f, particleID}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f} });
    }
    for (uint32_t i = 0; i < MAX_PARTICLES; ++i)
    {
        uint32_t startIndex = i * 4;
        vertexIndices.push_back(startIndex + 0);
        vertexIndices.push_back(startIndex + 1);
        vertexIndices.push_back(startIndex + 2);
        vertexIndices.push_back(startIndex + 2);
        vertexIndices.push_back(startIndex + 3);
        vertexIndices.push_back(startIndex + 0);
    }
}

void Particles::SetTransformations()
{
	transformations.position = glm::vec3(-6.0f, 0.0f, 0.0f);
	transformations.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	transformations.scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

void Particles::AssignTexures(const std::string texture_)
{
	texture = Texture(texture_);
}

void Particles::CreateVertexIndexBuffers(const CoreVulkan* coreVulkan, const CommandPoolVulkan* commandPoolVulkan)
{
    vulkanBuffers = VulkanBuffers(coreVulkan, commandPoolVulkan, particleMeshObject, vertexIndices);
    modelBuffersVulkan = &vulkanBuffers.GetModelBuffersVulkan();
}

void Particles::CreateTexture(const CoreVulkan* coreVulkan_, const CommandPoolVulkan* commandPoolVulkan_, const VkSampler* texSampler)
{
    texture.CreateTexture(coreVulkan_, commandPoolVulkan_, texSampler);
}

void Particles::CreateDescriptor(const CoreVulkan* coreVulkan_, const ModelBuffersVulkan* modelBuffersVulkan_, const PipelineVulkan* pipelineVulkan_, const UniformVulkan& uniformBufferObject)
{
    vulkanDescriptor = VulkanDescriptor(coreVulkan_, modelBuffersVulkan_, pipelineVulkan_, texture.GetTextureVulkan(), uniformBufferObject);
    descriptorVulkan = &vulkanDescriptor.GetDescriptorVulkan();
}

void Particles::CreateVulkanResources(const CoreVulkan* coreVulkan_, const CommandPoolVulkan* commandPoolVulkan_, const PipelineVulkan* pipelineVulkan_, const UniformVulkan& uniformBufferObject, const VkSampler* texSampler)
{
    AssignTexures("textures/particle.png"); // HARD-CODED FOR LACK OF TIME
    CreateVertexIndexBuffers(coreVulkan_, commandPoolVulkan_);
    CreateTexture(coreVulkan_, commandPoolVulkan_, texSampler);
    CreateDescriptor(coreVulkan_, modelBuffersVulkan, pipelineVulkan_, uniformBufferObject);
}

void Particles::UpdatePushConstants(VkCommandBuffer commandBuffer, const PipelineVulkan* pipelineVulkan) const
{
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, transformations.position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(transformations.rotation.x), glm::vec3(-1.0f, 0.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(transformations.rotation.y), glm::vec3(0.0f, -1.0f, 0.0f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(transformations.rotation.z), glm::vec3(0.0f, 0.0f, -1.0f));
    modelMatrix = glm::scale(modelMatrix, transformations.scale);
    ModelPushConstants pushConstants{};
    pushConstants.modelMatrix = modelMatrix;

    vkCmdPushConstants(commandBuffer,
        pipelineVulkan->pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(ModelPushConstants),
        &pushConstants
    );
}
