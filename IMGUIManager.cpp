#include "IMGUIManager.h"

static void check_vk_result(VkResult err)
{
	if (err == VK_SUCCESS)
		return;
	fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
	if (err < 0)
		abort();
}

void IMGUIManager::InitializeIMGUI()
{
	//IMGUI Setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();

	// Setup Platform/Renderer backends
	uint32_t graphicsQueueFamily = ImGui_ImplVulkanH_SelectQueueFamilyIndex(coreVulkan->physicalDevice);
	IM_ASSERT(graphicsQueueFamily != (uint32_t)-1);
	VkPipelineCache pipelineCache = VK_NULL_HANDLE;

	ImGui_ImplGlfw_InitForVulkan(window, true);
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance = coreVulkan->instance;
	init_info.PhysicalDevice = coreVulkan->physicalDevice;
	init_info.Device = coreVulkan->device;
	init_info.QueueFamily = graphicsQueueFamily;
	init_info.Queue = coreVulkan->graphicsQueue;
	init_info.PipelineCache = pipelineCache;
	init_info.DescriptorPool = mainDescriptorPool;
	init_info.MinImageCount = swapChainVulkan->imageCountMin;
	init_info.ImageCount = swapChainVulkan->imageCount;
	init_info.Allocator = nullptr;
	init_info.PipelineInfoMain.RenderPass = pipelineVulkan->renderPass;
	init_info.PipelineInfoMain.Subpass = 0;
	init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
	init_info.CheckVkResultFn = check_vk_result;

	ImGui_ImplVulkan_Init(&init_info);
}

void IMGUIManager::DisplayIMGUI(uint32_t currentFrame, float deltaTime, uint32_t vertexCount)
{
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_Once);

	ImGui::Begin("Graphics Statistics");
	ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::Text("Vertices Rendered: %u", vertexCount);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffersVulkan->commandBuffers[currentFrame]);
}

void IMGUIManager::CleanUp()
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}