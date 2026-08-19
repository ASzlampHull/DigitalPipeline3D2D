#include "SSBOBuffer.h"

void SSBOBuffer::CreateSSBOBuffer()
{
	ssboBufferSize = numElements * sizeof(uint32_t);

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = ssboBufferSize;
	bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	vkCreateBuffer(coreVulkan->device, &bufferInfo, nullptr, &ssboBuffer);

	VkMemoryRequirements memReqs;
	vkGetBufferMemoryRequirements(coreVulkan->device, ssboBuffer, &memReqs);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqs.size;
	allocInfo.memoryTypeIndex = VulkQuery::FindMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, coreVulkan->physicalDevice);

	vkAllocateMemory(coreVulkan->device, &allocInfo, nullptr, &ssboBufferMemory);

	vkBindBufferMemory(coreVulkan->device, ssboBuffer, ssboBufferMemory, 0);

	void* data;
	vkMapMemory(coreVulkan->device, ssboBufferMemory, 0, ssboBufferSize, 0, &data);
	memset(data, 0, ssboBufferSize);  // All values start at 0
	vkUnmapMemory(coreVulkan->device, ssboBufferMemory);
}

void SSBOBuffer::CreateDescriptorPool()
{
	const uint32_t maxSets = 1000;
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	poolSize.descriptorCount = maxSets;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.maxSets = maxSets;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;

	if (vkCreateDescriptorPool(coreVulkan->device, &poolInfo, nullptr, &descriptorVulkan.descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute descriptor pool!");
	}
}

void SSBOBuffer::CreateDescriptorSet()
{
	std::vector<VkDescriptorSetLayout> computeLayouts(MAX_FRAMES_IN_FLIGHT, pipelineVulkan->descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocSetInfo{};
	allocSetInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocSetInfo.descriptorPool = descriptorVulkan.descriptorPool;
	allocSetInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocSetInfo.pSetLayouts = computeLayouts.data();

	descriptorVulkan.descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(coreVulkan->device, &allocSetInfo, descriptorVulkan.descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate compute descriptor sets!");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = ssboBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet writeSet{};
		writeSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeSet.dstSet = descriptorVulkan.descriptorSets[i];
		writeSet.dstBinding = 0;           
		writeSet.descriptorCount = 1;
		writeSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeSet.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(coreVulkan->device, 1, &writeSet, 0, nullptr);
	}
}

void SSBOBuffer::DEBUG_PrintSSBOBufferInfo()
{
	void* mappedData;
	vkMapMemory(coreVulkan->device, ssboBufferMemory, 0, ssboBufferSize, 0, &mappedData);

	uint32_t* results = static_cast<uint32_t*>(mappedData);
	for (uint32_t i = 0; i < numElements; i++) {
		printf("values[%u] = %u\n", i, results[i]);
	}

	vkUnmapMemory(coreVulkan->device, ssboBufferMemory);
}

void SSBOBuffer::CleanUp()
{
	vkDestroyBuffer(coreVulkan->device, ssboBuffer, nullptr);
	vkFreeMemory(coreVulkan->device, ssboBufferMemory, nullptr);
}
