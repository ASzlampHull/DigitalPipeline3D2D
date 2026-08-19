#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <GLFW/glfw3.h>

const int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct CoreVulkan {
	VkInstance instance = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
	VkSurfaceKHR surface = VK_NULL_HANDLE;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;

	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue presentQueue = VK_NULL_HANDLE;

	CoreVulkan() = default;
	~CoreVulkan() = default;
	CoreVulkan(const CoreVulkan& other) = default;
	CoreVulkan& operator=(const CoreVulkan& rhs)
	{
		if (this != &rhs) {
			this->instance = rhs.instance;
			this->debugMessenger = rhs.debugMessenger;
			this->surface = rhs.surface;
			this->physicalDevice = rhs.physicalDevice;
			this->device = rhs.device;
			this->graphicsQueue = rhs.graphicsQueue;
			this->presentQueue = rhs.presentQueue;
		}
		return *this;
	}
};

struct SwapChainVulkan {
	std::vector<VkImage> swapChainImages = {};
	std::vector<VkImageView> swapChainImageViews = {};
	std::vector<VkFramebuffer> swapChainFramebuffers = {};
	VkExtent2D swapChainExtent = {};
	VkSwapchainKHR swapChain = VK_NULL_HANDLE;
	VkFormat swapChainImageFormat = VK_FORMAT_UNDEFINED;
	uint32_t imageCount = 0;
	uint32_t imageCountMin = 0;

	SwapChainVulkan() = default;
	~SwapChainVulkan() = default;
	SwapChainVulkan(const SwapChainVulkan& other) = default;
	SwapChainVulkan& operator=(const SwapChainVulkan& rhs)
	{
		if (this != &rhs) {
			this->swapChainImages = rhs.swapChainImages;
			this->swapChainImageViews = rhs.swapChainImageViews;
			this->swapChainFramebuffers = rhs.swapChainFramebuffers;
			this->swapChainExtent = rhs.swapChainExtent;
			this->swapChain = rhs.swapChain;
			this->swapChainImageFormat = rhs.swapChainImageFormat;
			this->imageCount = rhs.imageCount;
			this->imageCountMin = rhs.imageCountMin;
		}
		return *this;
	}
};

enum class PipelineType {
	Graphics,
	Compute,
	NONE
};

struct PipelineVulkan {
	VkRenderPass renderPass = VK_NULL_HANDLE;
	VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	VkPipeline pipeline = VK_NULL_HANDLE;
	PipelineType pipelineType = PipelineType::NONE;

	PipelineVulkan() = default;
	~PipelineVulkan() = default;
	PipelineVulkan(const PipelineVulkan& other) = default;
	PipelineVulkan& operator=(const PipelineVulkan& rhs)
	{
		if (this != &rhs) {
			this->renderPass = rhs.renderPass;
			this->descriptorSetLayout = rhs.descriptorSetLayout;
			this->pipelineLayout = rhs.pipelineLayout;
			this->pipeline = rhs.pipeline;
			this->pipelineType = rhs.pipelineType;
		}
		return *this;
	}
};

struct ShaderVulkan {
	VkShaderModule vertShaderModule = VK_NULL_HANDLE;
	VkShaderModule fragShaderModule = VK_NULL_HANDLE;
	VkShaderModule computeShaderModule = VK_NULL_HANDLE;

	ShaderVulkan() = default;
	~ShaderVulkan() = default;
	ShaderVulkan(const ShaderVulkan& other) = default;
	ShaderVulkan& operator=(const ShaderVulkan& rhs)
	{
		if (this != &rhs) {
			this->vertShaderModule = rhs.vertShaderModule;
			this->fragShaderModule = rhs.fragShaderModule;
			this->computeShaderModule = rhs.computeShaderModule;
		}
		return *this;
	}
};

struct CommandPoolVulkan {
	VkCommandPool commandPool = VK_NULL_HANDLE;

	CommandPoolVulkan() = default;
	~CommandPoolVulkan() = default;
	CommandPoolVulkan(const CommandPoolVulkan& other) = default;
	CommandPoolVulkan& operator=(const CommandPoolVulkan& rhs)
	{
		if (this != &rhs) {
			this->commandPool = rhs.commandPool;
		}
		return *this;
	}
};

struct TextureVulkan {
	VkImage textureImage = VK_NULL_HANDLE;
	VkDeviceMemory textureImageMemory = VK_NULL_HANDLE;
	VkImageView textureImageView = VK_NULL_HANDLE;
	const VkSampler* textureSampler = nullptr;

	TextureVulkan() = default;
	~TextureVulkan() = default;
	TextureVulkan(const TextureVulkan& other) = default;
	TextureVulkan& operator=(const TextureVulkan& rhs)
	{
		if (this != &rhs) {
			this->textureImage = rhs.textureImage;
			this->textureImageMemory = rhs.textureImageMemory;
			this->textureImageView = rhs.textureImageView;
			this->textureSampler = rhs.textureSampler;
		}
		return *this;
	}
};

struct DepthVulkan {
	VkImage depthImage = VK_NULL_HANDLE;
	VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
	VkImageView depthImageView = VK_NULL_HANDLE;

	DepthVulkan() = default;
	~DepthVulkan() = default;
	DepthVulkan(const DepthVulkan& other) = default;
	DepthVulkan& operator=(const DepthVulkan& rhs)
	{
		if (this != &rhs) {
			this->depthImage = rhs.depthImage;
			this->depthImageMemory = rhs.depthImageMemory;
			this->depthImageView = rhs.depthImageView;
		}
		return *this;
	}
};

struct BufferVulkan {
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory bufferMemory = VK_NULL_HANDLE;

	BufferVulkan() = default;
	~BufferVulkan() = default;
	BufferVulkan(const BufferVulkan& other) = default;
	BufferVulkan& operator=(const BufferVulkan& rhs)
	{
		if (this != &rhs) {
			this->buffer = rhs.buffer;
			this->bufferMemory = rhs.bufferMemory;
		}
		return *this;
	}
};

struct UniformVulkan {
	std::vector<VkBuffer> uniformBuffers = {};
	std::vector<VkDeviceMemory> uniformBuffersMemory = {};
	std::vector<void*> uniformBuffersMapped = {};
};

struct ModelBuffersVulkan {
	BufferVulkan vertexBuffer;
	BufferVulkan indexBuffer;
};

struct DescriptorVulkan {
	std::vector<VkDescriptorSet> descriptorSets = {};
	VkDescriptorPool descriptorPool = VK_NULL_HANDLE;

	DescriptorVulkan() = default;
	~DescriptorVulkan() = default;
	DescriptorVulkan(const DescriptorVulkan& other) = default;
	DescriptorVulkan& operator=(const DescriptorVulkan& rhs)
	{
		if (this != &rhs) {
			this->descriptorSets = rhs.descriptorSets;
			this->descriptorPool = rhs.descriptorPool;
		}
		return *this;
	}
};

struct CommandBuffersVulkan {
	std::vector<VkCommandBuffer> commandBuffers = {};
};

struct FrameSyncVulkan {
	std::vector<VkSemaphore> imageAvailableSemaphores = {};
	std::vector<VkSemaphore> renderFinishedSemaphores = {};
	std::vector<VkFence> inFlightFences = {};
};