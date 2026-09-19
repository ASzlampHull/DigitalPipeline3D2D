#include "FeatureReadback.h"

// Initialises the FeatureReadback system
void FeatureReadback::Create(const CoreVulkan* coreVulkan_, uint32_t queueFamilyIndex_, const Config& config_)
{
	coreVulkan = coreVulkan_;
    queueFamilyIndex = queueFamilyIndex_;
    config = config_;

    ReadbackTargetConfig targetConfig;
    targetConfig.width = config.width;
    targetConfig.height = config.height;
    targetConfig.colorFormat = config.colourFormat;
    targetConfig.depthFormat = config.depthFormat;
    targetConfig.frameCount = config.frameCount;
	readbackImageTarget = ReadbackImageTarget(coreVulkan, targetConfig);

	readbackBufferManager = ReadbackBufferManager(coreVulkan, config.width, config.height, config.frameCount, 4);
	depthBufferReader = DepthBufferReader(coreVulkan, config.width, config.height);
	readbackSyncroniser = ReadbackSynchroniser(coreVulkan, config.frameCount);
	pixelSegmentMerger.AddConfig(config.mergerConfig);
	segmentDeduplicator.AddConfig(config.deduplicatorConfig);
	featureSegmentValidator.AddConfig(config.validatorConfig);
}

// Calls the readback processes for a given frame index
// recording the color and depth images to buffers.
void FeatureReadback::RecordReadback(VkCommandBuffer commandBuffer, uint32_t frameIndex)
{
    readbackImageTarget.TransitionForReadback(commandBuffer);
    readbackBufferManager.RecordImageToBufferCopy(commandBuffer, frameIndex, readbackImageTarget.GetColourTexture().textureImage, config.width, config.height);
	depthBufferReader.RecordDepthCopy(commandBuffer, frameIndex, readbackImageTarget.GetDepthTexture().textureImage);
}

// Extract the feature segments from the readback buffers for a given frame index.
std::vector<FeatureSegment> FeatureReadback::ExtractSegments(uint32_t frameIndex, const std::function<glm::vec3(uint32_t)>& triangleNormalLookup, const glm::vec3& colourSampler, uint64_t timeoutNs)
{
    // Wait for GPU to finish
    if (!readbackSyncroniser.WaitForReadback(frameIndex, timeoutNs)) {
        throw std::runtime_error("Readback fence timeout");
    }

    // Decode color buffer -> PixelHits
    const uint8_t* colorData = static_cast<const uint8_t*>(readbackBufferManager.GetMapBufferForFrame(frameIndex));
	const float* depthData = depthBufferReader.DepthRow(frameIndex, 0);

    std::vector<PixelHit> hits = PixelDecoder::Decode(
        colorData, depthData,
        config.width, config.height,
        config.colourFormat);

    // Reconstruct world positions
	worldPositionReconstructor.ReconstructAll(hits, config.width, config.height);

    // Merge pixels -> segments
	std::vector<FeatureSegment> segments = pixelSegmentMerger.Merge(
        hits,
        config.width, config.height,
        triangleNormalLookup,
        eyePosition,
        colourSampler);

    // Deduplicate shared-edge segments
    segments = segmentDeduplicator.Deduplicate(segments);

    // Filter invalid segments
    segments = featureSegmentValidator.Filter(segments);

    // Reset fence for next frame
    readbackSyncroniser.ResetFence(frameIndex);

    return segments;
}

void FeatureReadback::CleanUp()
{
    depthBufferReader.CleanUp();
	readbackBufferManager.CleanUp();
	readbackImageTarget.CleanUp();
	readbackSyncroniser.CleanUp();
}
