#pragma once
#include "VulkanDataTypes.h"
#include "VulkanCore.h"
#include "FeatureReadbackTypes.h"
#include "SegmentDeduplicator.h"
#include "FeatureSegmentValidator.h"
#include "ReadbackImageTarget.h"
#include "ReadbackBufferManager.h"
#include "DepthBufferReader.h"
#include "ReadbackSynchroniser.h"
#include "PixelDecoder.h"
#include "PixelSegmentMerger.h"
#include "WorldPositionReconstructor.h"

// The main class that manages the entire feature readback process
// 1. Create a readback target (color + depth) and buffers for readback
// 2. Record readback commands after rendering into the target
// 3. Submit the readback commands and wait for completion using fences
// 4. Extract the pixel hits from the readback buffers
// 5. Decodes pixel hits
// 6. Reconstruct world positions
// 7. Merges segments
// 8. Deduplicate segments
// 9. Validates the final feature segments
// 10. Returns the final deduplicated and validated feature segments
class FeatureReadback
{
public:
	struct Config {
		uint32_t width = 2048;
		uint32_t height = 2048;
		VkFormat colourFormat = VK_FORMAT_R8G8B8A8_UNORM;
		VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
		uint32_t frameCount = 2;

		PixelSegmentMerger::Config mergerConfig;
		SegmentDeduplicator::Config deduplicatorConfig;
		FeatureSegmentValidator::Config validatorConfig;
	};

private:
	const CoreVulkan* coreVulkan = nullptr;
	uint32_t queueFamilyIndex;
	Config config;

	// Pipeline resources
	ReadbackImageTarget readbackImageTarget;
	ReadbackBufferManager readbackBufferManager;
	DepthBufferReader depthBufferReader;
	ReadbackSynchroniser readbackSyncroniser;
	PixelDecoder pixelDecoder;
	WorldPositionReconstructor worldPositionReconstructor;
	PixelSegmentMerger pixelSegmentMerger;
	SegmentDeduplicator segmentDeduplicator;
	FeatureSegmentValidator featureSegmentValidator;

	glm::vec3 eyePosition{ 0.0f };

public:
	FeatureReadback() = default;
	~FeatureReadback() = default;

	void Create(const CoreVulkan* coreVulkan_, uint32_t queueFamilyIndex_, const Config& config_);

	void RecordReadback(VkCommandBuffer commandBuffer, uint32_t frameIndex);
	void SubmitReadback(VkQueue queue, VkCommandBuffer commandBuffer, uint32_t frameIndex) { 
		readbackSyncroniser.BeginReadback(queue, commandBuffer, frameIndex); 
	}
	std::vector<FeatureSegment> ExtractSegments(uint32_t frameIndex, const std::function<glm::vec3(uint32_t)>& triangleNormalLookup, const glm::vec3& colorSampler, uint64_t timeoutNs = UINT64_MAX);
	
	// Accessors
	void SetTriangleData(const std::vector<glm::vec3>& vertices, const std::vector<uint32_t>& indices) { 
		pixelSegmentMerger.SetTriangleData(vertices, indices); 
	}
	void SetCamera(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& eyePosition_) {
		worldPositionReconstructor = WorldPositionReconstructor(view, projection);
		eyePosition = eyePosition_; 
	}
	Config GetConfig() { return config; }
	ReadbackImageTarget& GetRenderTarget() { return readbackImageTarget; }
	const PixelSegmentMerger GetMerger() { return pixelSegmentMerger; }

	void CleanUp();
};