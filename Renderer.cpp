#include "Renderer.h"

void Renderer::InitVulkan()
{
	vulkanCore = VulkanCore(window);
	coreVulkan = &vulkanCore.GetCoreVulkan();
	vulkanSwapChain = VulkanSwapChain(window, coreVulkan);
	swapChainVulkan = &vulkanSwapChain.GetSwapChainVulkan();
	vulkanPipeline = VulkanPipeline(coreVulkan, swapChainVulkan);
	pipelineVulkan = &vulkanPipeline.GetPipelineVulkan();
	computePipelineVulkan = &vulkanPipeline.GetComputePipelineVulkan();
	outlinePipelineVulkan = &vulkanPipeline.GetOutlinePipelineVulkan();
	vulkanFramebuffers = VulkanFramebuffers(coreVulkan, const_cast<SwapChainVulkan*>(&vulkanSwapChain.GetSwapChainVulkan()), pipelineVulkan);
	commandPoolVulkan = &vulkanFramebuffers.GetCommandPoolVulkan();
	
	resourceManager.CreateVertexIndexBuffers(coreVulkan, commandPoolVulkan);
    CreateUniformBuffers();
    resourceManager.CreateTextures(coreVulkan, commandPoolVulkan, pipelineVulkan, uniformBufferObject);

	//Compute pipeline resources
	//ssboBuffer = SSBOBuffer(coreVulkan, computePipelineVulkan);
	postProcessingLines = PostProcessingLines(coreVulkan, computePipelineVulkan, swapChainVulkan);

	vulkanCommandBuffers = VulkanCommandBuffers(coreVulkan, commandPoolVulkan);
	commandBuffersVulkan = &vulkanCommandBuffers.GetCommandBuffersVulkan();
	vulkanFrameSync = VulkanFrameSync(coreVulkan);
	frameSyncVulkan = &vulkanFrameSync.GetFrameSyncVulkan();
}

void Renderer::InitIMGUI()
{
	imguiManager = IMGUIManager(
		window,
		coreVulkan,
		swapChainVulkan,
		resourceManager.GetMainDescriptorPool(),
		pipelineVulkan,
		commandPoolVulkan,
		commandBuffersVulkan);
}

void Renderer::CreateUniformBuffers()
{
    const VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBufferObject.uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBufferObject.uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    uniformBufferObject.uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VulkCreate::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBufferObject.uniformBuffers[i], uniformBufferObject.uniformBuffersMemory[i], coreVulkan->device, coreVulkan->physicalDevice);

        vkMapMemory(coreVulkan->device, uniformBufferObject.uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBufferObject.uniformBuffersMapped[i]);
    }
}

void Renderer::RecreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(coreVulkan->device);

	CleanupSwapChain();

	vulkanSwapChain.InitialiseSwapChain();
    vulkanFramebuffers.CreateDepthResources();
    vulkanFramebuffers.CreateFramebuffers();
}

void Renderer::CleanupSwapChain()
{
	vulkanFramebuffers.CleanupDepth();
    vulkanSwapChain.Cleanup();
}

void Renderer::Cleanup()
{
    imguiManager.CleanUp();

    CleanupSwapChain();

	vulkanPipeline.Cleanup();
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(coreVulkan->device, uniformBufferObject.uniformBuffers[i], nullptr);
        vkFreeMemory(coreVulkan->device, uniformBufferObject.uniformBuffersMemory[i], nullptr);
    }
	//ssboBuffer.CleanUp();
	postProcessingLines.CleanUp();
	resourceManager.CleanupTextures(coreVulkan);
    vulkanPipeline.CleanupDescriptorSetLayout();
    resourceManager.CleanupBuffersVI();
	vulkanFrameSync.Cleanup();
	vulkanFramebuffers.CleanupCommandPool();
    vulkanCore.Cleanup();
    
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

#pragma region Main Render Pass

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pipelineVulkan->renderPass;
    renderPassInfo.framebuffer = swapChainVulkan->swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChainVulkan->swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { {0.0f, 1.0f, 0.0f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChainVulkan->swapChainExtent.width);
    viewport.height = static_cast<float>(swapChainVulkan->swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = swapChainVulkan->swapChainExtent;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

#pragma region NormalPass

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineVulkan->pipeline);

	uint32_t vertexCount = 0;

	for (const auto& pair : resourceManager.GetModels()) {
        const auto& model = pair.second;
		const auto& mesh = model.GetMesh();
        const auto& descriptorVulkan = model.GetMaterial().GetDescriptorVulkan();
        const auto& modelBuffersVulkan = model.GetModelBuffersVulkan();

        std::array<VkBuffer, 1> vertexBuffers = { modelBuffersVulkan->vertexBuffer.buffer };
        std::array<VkDeviceSize, 1> offsets = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, static_cast<uint32_t>(vertexBuffers.size()), vertexBuffers.data(), offsets.data());
        vkCmdBindIndexBuffer(commandBuffer, modelBuffersVulkan->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        const uint32_t indexCount = static_cast<uint32_t>(mesh.GetVertexIndices().size());

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineVulkan->pipelineLayout,
            0, 1,
            &descriptorVulkan->descriptorSets[currentFrame],
            0, nullptr
        );

        model.UpdatePushConstants(commandBuffer, pipelineVulkan);
		//vkCmdSetStencilReference(commandBuffer, VK_STENCIL_FACE_FRONT_AND_BACK, 1); // Stencil reference value for the model
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);

        vertexCount += static_cast<uint32_t>(mesh.GetVertices().size());
    }

#pragma endregion

    /*
#pragma region OutlinePass

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, outlinePipelineVulkan->pipeline);

    for (const auto& pair : resourceManager.GetModels()) {
        auto model = pair.second;
        const auto& mesh = model.GetMesh();
        const auto& descriptorVulkan = model.GetMaterial().GetDescriptorVulkan();
        const auto& modelBuffersVulkan = model.GetModelBuffersVulkan();
        std::array<VkBuffer, 1> vertexBuffers = { modelBuffersVulkan->vertexBuffer.buffer };
        std::array<VkDeviceSize, 1> offsets = { 0 };

        vkCmdBindVertexBuffers(commandBuffer, 0, static_cast<uint32_t>(vertexBuffers.size()), vertexBuffers.data(), offsets.data());
        vkCmdBindIndexBuffer(commandBuffer, modelBuffersVulkan->indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        const uint32_t indexCount = static_cast<uint32_t>(mesh.GetVertexIndices().size());

        vkCmdBindDescriptorSets(
            commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            outlinePipelineVulkan->pipelineLayout,
            0, 1,
            &descriptorVulkan->descriptorSets[currentFrame],
            0, nullptr
        );

        Transformations modelTransfrom = model.GetTransformations();
		modelTransfrom.scale *= 1.05f; // Scale up for outline effect
        model.SetTransformations(modelTransfrom);

        model.UpdatePushConstants(commandBuffer, outlinePipelineVulkan);
        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);

		modelTransfrom.scale /= 1.05f; // Reset scale back to original
		model.SetTransformations(modelTransfrom);
    }

#pragma endregion
    */

	// Render IMGUI
    //if (displayIMGUI)
    //    imguiManager.DisplayIMGUI(currentFrame, deltaTime, vertexCount);

    vkCmdEndRenderPass(commandBuffer);

#pragma endregion

#pragma region PostProcessingPass

	// Swapchain and output image ready for Compute shader processing
    VkImageMemoryBarrier2 preComputeBarrierInput = VulkSync::CreateImageMemoryBarrier(
        swapChainVulkan->swapChainImages[imageIndex],
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
	VkDependencyInfo depInfo2 = VulkSync::CreateDependencyInfo(VK_STRUCTURE_TYPE_DEPENDENCY_INFO, 0, nullptr, nullptr, &preComputeBarrierInput, 0, 0, 1);
    vkCmdPipelineBarrier2(commandBuffer, &depInfo2);

    VkImageMemoryBarrier2 preComputeBarrierOutput = VulkSync::CreateImageMemoryBarrier(
		postProcessingLines.GetOutputScreenTexture().textureImage,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_ACCESS_2_NONE,
		VK_ACCESS_2_SHADER_WRITE_BIT,
		VK_PIPELINE_STAGE_2_NONE,
		VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT
	);
	VkDependencyInfo depInfo3 = VulkSync::CreateDependencyInfo(VK_STRUCTURE_TYPE_DEPENDENCY_INFO, 0, nullptr, nullptr, &preComputeBarrierOutput, 0, 0, 1);
    vkCmdPipelineBarrier2(commandBuffer, &depInfo3);

	// Bind the output image to the compute shader descriptor set
	postProcessingLines.UpdateInputDescriptorForCurrentImage(currentFrame, imageIndex);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineVulkan->pipeline);
    vkCmdBindDescriptorSets(commandBuffer, 
        VK_PIPELINE_BIND_POINT_COMPUTE,
        computePipelineVulkan->pipelineLayout,
        0, 1, &postProcessingLines.GetDescriptorVulkan().descriptorSets[currentFrame],
        0, nullptr);
	uint32_t local_sizeX = 8; // Must match the local_size_x in your compute shader
	uint32_t local_sizeY = 8; // Must match the local_size_y in your compute shader
	uint32_t groupCountX = (swapChainVulkan->swapChainExtent.width + local_sizeX - 1) / local_sizeX;
	uint32_t groupCountY = (swapChainVulkan->swapChainExtent.height + local_sizeY - 1) / local_sizeY;
    vkCmdDispatch(commandBuffer, groupCountX, groupCountY, 1);

	// Transition the output image to be ready for transfer back to the swapchain
	VkImageMemoryBarrier2 postComputeBarrierOutput = VulkSync::CreateImageMemoryBarrier(
		postProcessingLines.GetOutputScreenTexture().textureImage,
		VK_IMAGE_LAYOUT_GENERAL,
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_ACCESS_2_SHADER_WRITE_BIT,
		VK_ACCESS_2_TRANSFER_READ_BIT,
		VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_2_TRANSFER_BIT,
		VK_IMAGE_ASPECT_COLOR_BIT
	);
	VkDependencyInfo depInfo4 = VulkSync::CreateDependencyInfo(VK_STRUCTURE_TYPE_DEPENDENCY_INFO, 0, nullptr, nullptr, &postComputeBarrierOutput, 0, 0, 1);
	vkCmdPipelineBarrier2(commandBuffer, &depInfo4);

    VkImageMemoryBarrier2 postComputeBarrierInput = VulkSync::CreateImageMemoryBarrier(
        swapChainVulkan->swapChainImages[imageIndex],
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_ACCESS_2_SHADER_READ_BIT,
        VK_ACCESS_2_TRANSFER_WRITE_BIT,
        VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
    VkDependencyInfo depInfo5 = VulkSync::CreateDependencyInfo(VK_STRUCTURE_TYPE_DEPENDENCY_INFO, 0, nullptr, nullptr, &postComputeBarrierInput, 0, 0, 1);
    vkCmdPipelineBarrier2(commandBuffer, &depInfo5);

    // Copy output image to swapchain
    VkImageCopy copyRegion{};
    copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.srcSubresource.layerCount = 1;
    copyRegion.srcSubresource.mipLevel = 0;
    copyRegion.srcSubresource.baseArrayLayer = 0;
    copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.dstSubresource.layerCount = 1;
    copyRegion.dstSubresource.mipLevel = 0;
    copyRegion.dstSubresource.baseArrayLayer = 0;
    copyRegion.extent.width = swapChainVulkan->swapChainExtent.width;
    copyRegion.extent.height = swapChainVulkan->swapChainExtent.height;
    copyRegion.extent.depth = 1;
    vkCmdCopyImage(commandBuffer,
        postProcessingLines.GetOutputScreenTexture().textureImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        swapChainVulkan->swapChainImages[imageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, &copyRegion);

	// Transition swapchain image back to present layout
    VkImageMemoryBarrier2 presentBarrier = VulkSync::CreateImageMemoryBarrier(
        swapChainVulkan->swapChainImages[imageIndex],
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_ACCESS_2_TRANSFER_WRITE_BIT,
        VK_ACCESS_2_NONE,
        VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT
    );
	VkDependencyInfo depInfo6 = VulkSync::CreateDependencyInfo(VK_STRUCTURE_TYPE_DEPENDENCY_INFO, 0, nullptr, nullptr, &presentBarrier, 0, 0, 1);
	vkCmdPipelineBarrier2(commandBuffer, &depInfo6);

#pragma endregion

#pragma region Compute Pass

    //ComputePass(commandBuffer);

#pragma endregion

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void Renderer::ComputePass(VkCommandBuffer commandBuffer)
{
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineVulkan->pipeline);
    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        computePipelineVulkan->pipelineLayout,
        0, 1, ssboBuffer.GetDescriptorVulkan().descriptorSets.data() + currentFrame,
        0, nullptr
    );

    // Optionally push constants if used
    // vkCmdPushConstants(commandBuffer, computePipelineVulkan.pipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(...), &yourData);

    vkCmdDispatch(commandBuffer, ssboBuffer.GetNumElements(), 1, 1);

    VkBufferMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_SHADER_READ_BIT;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.buffer = ssboBuffer.GetSSBOBuffer();
    barrier.offset = 0;
    barrier.size = VK_WHOLE_SIZE;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, //VK_PIPELINE_STAGE_HOST_BIT,//
        0,
        0, nullptr,
        1, &barrier,
        0, nullptr);
}


void Renderer::UpdateUniformBuffer(uint32_t currentImage)
{
    UniformBufferObject ubo{};
    ubo.model = glm::mat4(1.0f);
	ubo.eyePos = currentCamera.eyePosition;
    ubo.view = currentCamera.viewMatrix;
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChainVulkan->swapChainExtent.width / static_cast<float>(swapChainVulkan->swapChainExtent.height), 0.1f, 2000.0f);
    ubo.proj[1][1] *= -1;
	ubo.time = timeAccumulator;

    // Spin sunDirection around the Y axis in a circle
    float sunAngle = timeAccumulator * 0.5f; // 0.5f = speed, adjust as needed
    float radius = 1.0f; // Length of the direction vector

    ubo.sunDirection = glm::normalize(glm::vec3(
        radius * std::sin(sunAngle), // X
        -1.0f,                       // Y (keep sun above/below scene)
        radius * std::cos(sunAngle)  // Z
    ));

    ubo.sunLightColor = glm::vec3(1.0f);
	
    memcpy(uniformBufferObject.uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

void Renderer::DrawFrame()
{
    vkWaitForFences(coreVulkan->device, 1, &frameSyncVulkan->inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(coreVulkan->device, swapChainVulkan->swapChain, UINT64_MAX, frameSyncVulkan->imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        RecreateSwapChain();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    UpdateUniformBuffer(currentFrame);

    vkResetFences(coreVulkan->device, 1, &frameSyncVulkan->inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffersVulkan->commandBuffers[currentFrame], /*VkCommandBufferResetFlagBits*/ 0);
    RecordCommandBuffer(commandBuffersVulkan->commandBuffers[currentFrame], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::array<VkSemaphore, 1> waitSemaphores = { frameSyncVulkan->imageAvailableSemaphores[currentFrame] };
    std::array<VkPipelineStageFlags, 1> waitStages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    submitInfo.pWaitSemaphores = waitSemaphores.data();
    submitInfo.pWaitDstStageMask = waitStages.data();

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffersVulkan->commandBuffers[currentFrame];

    std::array<VkSemaphore, 1> signalSemaphores = { frameSyncVulkan->renderFinishedSemaphores[currentFrame] };
    submitInfo.signalSemaphoreCount = static_cast<uint32_t>(signalSemaphores.size());
    submitInfo.pSignalSemaphores = signalSemaphores.data();

    if (vkQueueSubmit(coreVulkan->graphicsQueue, 1, &submitInfo, frameSyncVulkan->inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores.data();

    std::array<VkSwapchainKHR, 1> swapChains = { swapChainVulkan->swapChain };
    presentInfo.swapchainCount = static_cast<uint32_t>(swapChains.size());
    presentInfo.pSwapchains = swapChains.data();

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(coreVulkan->presentQueue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        *framebufferResized = false;
        RecreateSwapChain();
    }
    else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::InitRenderer(const ConfigData& configData, GLFWwindow* window_, const CameraSettings& currentCamera_)
{
    window = window_;
	currentCamera = currentCamera_;
    resourceManager = ResourceManager(configData);
    InitVulkan();
	InitIMGUI();
}

void Renderer::Update(const InputManager& input, const CameraSettings& currentCamera_, float deltaTime_, const bool* framebufferResized_)
{
    currentCamera = currentCamera_;
	framebufferResized = const_cast<bool*>(framebufferResized_);
    deltaTime = deltaTime_;
	timeAccumulator += deltaTime;

	if (input.IsCommandPressed(Commands::DisplayIMGUI)) {
		displayIMGUI = !displayIMGUI;
	}

    DrawFrame();
}
