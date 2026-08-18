#pragma once
#include "ResourceManager.h"
#include "WindowManager.h"
#include "Camera.h"
#include "VulkanCore.h"
#include "VulkanSwapChain.h"
#include "VulkanPipeline.h"
#include "VulkanFramebuffers.h"
#include "VulkanBuffers.h"
#include "VulkanCommandBuffers.h"
#include "VulkanFrameSync.h"
#include "SSBOBuffer.h"
#include "IMGUIManager.h"
#include <chrono>
#include <array>
#include <GLFW/glfw3.h>


class Renderer final {
private:
    ResourceManager resourceManager;
	IMGUIManager imguiManager;

    VulkanCore vulkanCore;
    VulkanSwapChain vulkanSwapChain;
    VulkanPipeline vulkanPipeline;
    VulkanFramebuffers vulkanFramebuffers;
    VulkanCommandBuffers vulkanCommandBuffers;
    VulkanFrameSync vulkanFrameSync;

    UniformVulkan uniformBufferObject;
    CameraSettings currentCamera;

    GLFWwindow* window;
    const CoreVulkan* coreVulkan;
    const SwapChainVulkan* swapChainVulkan;
    const PipelineVulkan* pipelineVulkan;
	const PipelineVulkan* computePipelineVulkan;
    const CommandPoolVulkan* commandPoolVulkan;
    const CommandBuffersVulkan* commandBuffersVulkan;
    const FrameSyncVulkan* frameSyncVulkan;
    bool* framebufferResized;
	bool displayIMGUI = true;

	// Compute resources
	SSBOBuffer ssboBuffer;

    uint32_t currentFrame = 0;
    float deltaTime = 0.0f;
    float timeAccumulator = 0.0f;

	void InitVulkan();
	void InitIMGUI();
	void CreateUniformBuffers();
	void RecreateSwapChain();
	void CleanupSwapChain();
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void UpdateUniformBuffer(uint32_t currentImage);
	void DrawFrame();
public:
	Renderer() = default;
	~Renderer() = default;
	Renderer(const Renderer& other) = default;
    Renderer& operator=(const Renderer& other)
    {
        if (this != &other) {
            resourceManager = other.resourceManager;
			imguiManager = other.imguiManager;
            vulkanCore = other.vulkanCore;
            vulkanSwapChain = other.vulkanSwapChain;
            vulkanPipeline = other.vulkanPipeline;
            vulkanFramebuffers = other.vulkanFramebuffers;
            vulkanCommandBuffers = other.vulkanCommandBuffers;
            vulkanFrameSync = other.vulkanFrameSync;
            uniformBufferObject = other.uniformBufferObject;
            currentCamera = other.currentCamera;
            window = other.window;
            coreVulkan = other.coreVulkan;
            swapChainVulkan = other.swapChainVulkan;
            pipelineVulkan = other.pipelineVulkan;
			computePipelineVulkan = other.computePipelineVulkan;
            commandPoolVulkan = other.commandPoolVulkan;
            commandBuffersVulkan = other.commandBuffersVulkan;
            frameSyncVulkan = other.frameSyncVulkan;
            framebufferResized = other.framebufferResized;
			displayIMGUI = other.displayIMGUI;
            currentFrame = other.currentFrame;
            deltaTime = other.deltaTime;
            timeAccumulator = other.timeAccumulator;
			ssboBuffer = other.ssboBuffer;
        }
        return *this;
    }

	const ResourceManager& GetResourceManager() const { return resourceManager; };
	const CoreVulkan& GetCoreVulkan() const { return *coreVulkan; };

	void InitRenderer(const ConfigData& configData, GLFWwindow* window_, const CameraSettings& currentCamera_);
	void Update(const InputManager& input, const CameraSettings& currentCamera_, float deltaTime_, const bool* framebufferResized_);
	void Cleanup();
};