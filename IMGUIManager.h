#pragma once

#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
// parasoft-begin-suppress ALL "suppress all violations"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
// parasoft-end-suppress ALL "suppress all violations"

class IMGUIManager final {
private:
	const CoreVulkan* coreVulkan = nullptr;
	const SwapChainVulkan* swapChainVulkan = nullptr;
	const PipelineVulkan* pipelineVulkan = nullptr;
	const CommandPoolVulkan* commandPoolVulkan = nullptr;
	const CommandBuffersVulkan* commandBuffersVulkan = nullptr;
	VkDescriptorPool mainDescriptorPool = VK_NULL_HANDLE;
	GLFWwindow* window = nullptr;

	void InitializeIMGUI();

public:
	IMGUIManager() = default;
	~IMGUIManager() = default;
	IMGUIManager(const IMGUIManager& other) = delete;
	IMGUIManager& operator=(const IMGUIManager& other)
	{
		if (this != &other) {
			this->coreVulkan = other.coreVulkan;
			this->swapChainVulkan = other.swapChainVulkan;
			this->pipelineVulkan = other.pipelineVulkan;
			this->commandPoolVulkan = other.commandPoolVulkan;
			this->commandBuffersVulkan = other.commandBuffersVulkan;
			this->mainDescriptorPool = other.mainDescriptorPool;
			this->window = other.window;
		}
		return *this;
	}

	IMGUIManager(const GLFWwindow* window_, 
		const CoreVulkan* coreVulkan_, 
		const SwapChainVulkan* swapChainVulkan_, 
		const VkDescriptorPool descriptorVulkanPool_,
		const PipelineVulkan* pipelineVulkan_,
		const CommandPoolVulkan* commandPoolVulkan_,
		const CommandBuffersVulkan* commandBuffersVulkan_ = nullptr)
		: window(const_cast<GLFWwindow*>(window_)),
		coreVulkan(coreVulkan_),
		swapChainVulkan(swapChainVulkan_),
		mainDescriptorPool(descriptorVulkanPool_),
		pipelineVulkan(pipelineVulkan_),
		commandPoolVulkan(commandPoolVulkan_),
		commandBuffersVulkan(commandBuffersVulkan_)
	{
		InitializeIMGUI();
	}

	void DisplayIMGUI(uint32_t currentFrame, float deltaTime, uint32_t vertexCount);
	void CleanUp();
};

