#include "VulkanDescriptor.h"

void VulkanDescriptor::InitialiseDescriptor()
{
	CreateDescriptorPool();
	CreateDescriptorSets();
}

void VulkanDescriptor::CreateDescriptorPool()
{
    switch (pipelineVulkan->pipelineType) {
        case PipelineType::Graphics: {
            const uint32_t maxSets = 1000;
            std::array<VkDescriptorPoolSize, 3> poolSizes{};
            // Uniform buffer descriptor
            poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            poolSizes[0].descriptorCount = maxSets;
            // Combined image sampler descriptor
            poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            poolSizes[1].descriptorCount = maxSets;
			// 1D storage buffer descriptor for cel shading
			poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[2].descriptorCount = maxSets;

            VkDescriptorPoolCreateInfo poolInfo{};
            poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
            poolInfo.pPoolSizes = poolSizes.data();
            poolInfo.maxSets = maxSets;

            if (vkCreateDescriptorPool(coreVulkan->device, &poolInfo, nullptr, &descriptorVulkan.descriptorPool) != VK_SUCCESS) {
                throw std::runtime_error("failed to create descriptor pool!");
            }
            break;
        }
        case PipelineType::Compute:{
            const uint32_t maxComputeSets = 1000;
            VkDescriptorPoolSize computePoolSize{};
            computePoolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            computePoolSize.descriptorCount = maxComputeSets;

            VkDescriptorPoolCreateInfo computePoolInfo{};
            computePoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
            computePoolInfo.poolSizeCount = 1;
            computePoolInfo.pPoolSizes = &computePoolSize;
            computePoolInfo.maxSets = maxComputeSets;
            if (vkCreateDescriptorPool(coreVulkan->device, &computePoolInfo, nullptr, &descriptorVulkan.descriptorPool) != VK_SUCCESS) {
                throw std::runtime_error("failed to create compute descriptor pool!");
            }
            break;
        }
	    case PipelineType::NONE:{
		    throw std::runtime_error("Pipeline type is NONE, cannot create descriptor pool!");
		    break;
        }
        default: {
            throw std::runtime_error("Unknown pipeline type, cannot create descriptor pool!");
            break;
        }
    }
}

void VulkanDescriptor::CreateDescriptorSets()
{
    switch (pipelineVulkan->pipelineType) {
        case PipelineType::Graphics:{
            std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, pipelineVulkan->descriptorSetLayout);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = descriptorVulkan.descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            allocInfo.pSetLayouts = layouts.data();

            descriptorVulkan.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
            if (vkAllocateDescriptorSets(coreVulkan->device, &allocInfo, descriptorVulkan.descriptorSets.data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate descriptor sets!");
            }

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformBufferObject.uniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = textureVulkan->textureImageView;
                imageInfo.sampler = *textureVulkan->textureSampler;

				VkDescriptorImageInfo celShadingImageInfo{};
				celShadingImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                celShadingImageInfo.imageView = textureVulkanCel->textureImageView;
				celShadingImageInfo.sampler = *textureVulkanCel->textureSampler;

                std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

                // Write descriptor for uniform buffer
                descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[0].dstSet = descriptorVulkan.descriptorSets[i];
                descriptorWrites[0].dstBinding = 0;
                descriptorWrites[0].dstArrayElement = 0;
                descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorWrites[0].descriptorCount = 1;
                descriptorWrites[0].pBufferInfo = &bufferInfo;

                // Write descriptor for combined image sampler
                descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                descriptorWrites[1].dstSet = descriptorVulkan.descriptorSets[i];
                descriptorWrites[1].dstBinding = 1;
                descriptorWrites[1].dstArrayElement = 0;
                descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorWrites[1].descriptorCount = 1;
                descriptorWrites[1].pImageInfo = &imageInfo;

				// Write descriptor for cel shading combined image sampler
				descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[2].dstSet = descriptorVulkan.descriptorSets[i];
				descriptorWrites[2].dstBinding = 2;
				descriptorWrites[2].dstArrayElement = 0;
				descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[2].descriptorCount = 1;
				descriptorWrites[2].pImageInfo = &celShadingImageInfo;

                vkUpdateDescriptorSets(coreVulkan->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
            }
            break;
        }
        case PipelineType::Compute: {
            std::vector<VkDescriptorSetLayout> computeLayouts(MAX_FRAMES_IN_FLIGHT, pipelineVulkan->descriptorSetLayout);
            VkDescriptorSetAllocateInfo computeAllocInfo{};
            computeAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            computeAllocInfo.descriptorPool = descriptorVulkan.descriptorPool;
            computeAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
            computeAllocInfo.pSetLayouts = computeLayouts.data();

            descriptorVulkan.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
            if (vkAllocateDescriptorSets(coreVulkan->device, &computeAllocInfo, descriptorVulkan.descriptorSets.data()) != VK_SUCCESS) {
                throw std::runtime_error("failed to allocate compute descriptor sets!");
            }
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                VkDescriptorBufferInfo bufferInfo{};
                bufferInfo.buffer = uniformBufferObject.uniformBuffers[i];
                bufferInfo.offset = 0;
                bufferInfo.range = sizeof(UniformBufferObject);

                VkWriteDescriptorSet computeDescriptorWrite{};
                computeDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                computeDescriptorWrite.dstSet = descriptorVulkan.descriptorSets[i];
                computeDescriptorWrite.dstBinding = 0;
                computeDescriptorWrite.dstArrayElement = 0;
                computeDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                computeDescriptorWrite.descriptorCount = 1;
                computeDescriptorWrite.pBufferInfo = &bufferInfo;

                vkUpdateDescriptorSets(coreVulkan->device, 1, &computeDescriptorWrite, 0, nullptr);
            }
            break;
        }
	    case PipelineType::NONE: {
		    throw std::runtime_error("Pipeline type is NONE, cannot create descriptor sets!");
		    break;
        }
        default: {
            throw std::runtime_error("Unknown pipeline type, cannot create descriptor sets!");
            break;
        }
    }
}