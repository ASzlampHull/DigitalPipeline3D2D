#include "ReadbackSynchroniser.h"

void ReadbackSynchroniser::Initialise()
{
	fences.resize(frameCount);

	for (uint32_t i = 0; i < frameCount; i++)
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		if (vkCreateFence(coreVulkan->device, &fenceInfo, nullptr, &fences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create fence!");
		}
	}
}

// Called after the render command buffer is submitted.
// Returns the fence that will be signalled when readback is done.
void ReadbackSynchroniser::BeginReadback(VkQueue queue, VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
	vkResetFences(coreVulkan->device, 1, &fences[frameIndex]);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, fences[frameIndex]);
}


bool ReadbackSynchroniser::WaitForReadback(uint32_t frameIndex, uint64_t timeoutNs)
{
	VkResult result = vkWaitForFences(coreVulkan->device, 1, &fences[frameIndex], VK_TRUE, timeoutNs);
	return result == VK_SUCCESS;
}

bool ReadbackSynchroniser::IsReadbackComplete(uint32_t frameIndex) const
{
	VkResult result = vkGetFenceStatus(coreVulkan->device, fences[frameIndex]);
	return result == VK_SUCCESS;
}

void ReadbackSynchroniser::ResetFence(uint32_t frameIndex)
{
	vkResetFences(coreVulkan->device, 1, &fences[frameIndex]);
}

void ReadbackSynchroniser::CleanUp()
{
	for (uint32_t i = 0; i < frameCount; i++)
	{
		vkDestroyFence(coreVulkan->device, fences[i], nullptr);
	}
	fences.clear();
}
