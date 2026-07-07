#pragma once

#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "RenderDataTypes.h"
#include "ModelParserDataTypes.h"

class VulkanBuffers final {
private:
	ModelBuffersVulkan modelBuffersVulkan = {};
	const CoreVulkan* coreVulkan = nullptr;
	const CommandPoolVulkan* commandPoolVulkan = nullptr;

	void InitialiseBuffers(const MeshObject& meshObject, const IndicesVector& indices);
	void CreateVertexBuffer(const MeshObject& meshObject);
	void CreateIndexBuffer(const IndicesVector& indices);
	
	const void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) const;
public:
	VulkanBuffers() = default;
	~VulkanBuffers() = default;
	VulkanBuffers(const CoreVulkan* coreVulkan_, const CommandPoolVulkan* commandPoolVulkan_, const MeshObject& meshObject, const IndicesVector& indices)
		: coreVulkan(coreVulkan_), commandPoolVulkan(commandPoolVulkan_)
	{
		InitialiseBuffers(meshObject, indices);
	}
	VulkanBuffers(const VulkanBuffers& other) = default;
	VulkanBuffers& operator=(const VulkanBuffers& rhs)
	{
		if (this != &rhs) {
			this->coreVulkan = rhs.coreVulkan;
			this->commandPoolVulkan = rhs.commandPoolVulkan;
			this->modelBuffersVulkan = rhs.modelBuffersVulkan;
		}
		return *this;
	}

	const ModelBuffersVulkan& GetModelBuffersVulkan() const { return modelBuffersVulkan; };	

	const void CleanupBuffersVI() const;
};