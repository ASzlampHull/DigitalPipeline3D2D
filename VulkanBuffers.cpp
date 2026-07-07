#include "VulkanBuffers.h"

void VulkanBuffers::InitialiseBuffers(const MeshObject& meshObject, const IndicesVector& indices)
{
	CreateVertexBuffer(meshObject);
	CreateIndexBuffer(indices);
}

void VulkanBuffers::CreateVertexBuffer(const MeshObject& meshObject)
{
    const VkDeviceSize bufferSize = sizeof(meshObject[0]) * meshObject.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VulkCreate::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, coreVulkan->device, coreVulkan->physicalDevice);

    void* data;
    vkMapMemory(coreVulkan->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, meshObject.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(coreVulkan->device, stagingBufferMemory);

    VulkCreate::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, modelBuffersVulkan.vertexBuffer.buffer, modelBuffersVulkan.vertexBuffer.bufferMemory, coreVulkan->device, coreVulkan->physicalDevice);

    CopyBuffer(stagingBuffer, modelBuffersVulkan.vertexBuffer.buffer, bufferSize);

    vkDestroyBuffer(coreVulkan->device, stagingBuffer, nullptr);
    vkFreeMemory(coreVulkan->device, stagingBufferMemory, nullptr);
}

void VulkanBuffers::CreateIndexBuffer(const IndicesVector& indices)
{
    const VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VulkCreate::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory, coreVulkan->device, coreVulkan->physicalDevice);

    void* data;
    vkMapMemory(coreVulkan->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(coreVulkan->device, stagingBufferMemory);

    VulkCreate::CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, modelBuffersVulkan.indexBuffer.buffer, modelBuffersVulkan.indexBuffer.bufferMemory, coreVulkan->device, coreVulkan->physicalDevice);

    CopyBuffer(stagingBuffer, modelBuffersVulkan.indexBuffer.buffer, bufferSize);

    vkDestroyBuffer(coreVulkan->device, stagingBuffer, nullptr);
    vkFreeMemory(coreVulkan->device, stagingBufferMemory, nullptr);
}

const void VulkanBuffers::CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const
{
    const VkCommandBuffer commandBuffer = VulkCommands::BeginSingleTimeCommands(commandPoolVulkan->commandPool, coreVulkan->device);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    VulkCommands::EndSingleTimeCommands(commandBuffer, commandPoolVulkan->commandPool, coreVulkan->graphicsQueue, coreVulkan->device);
}

const void VulkanBuffers::CleanupBuffersVI() const
{
	vkDestroyBuffer(coreVulkan->device, modelBuffersVulkan.indexBuffer.buffer, nullptr);
	vkFreeMemory(coreVulkan->device, modelBuffersVulkan.indexBuffer.bufferMemory, nullptr);

	vkDestroyBuffer(coreVulkan->device, modelBuffersVulkan.vertexBuffer.buffer, nullptr);
	vkFreeMemory(coreVulkan->device, modelBuffersVulkan.vertexBuffer.bufferMemory, nullptr);
}
