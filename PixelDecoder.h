#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "FeatureReadbackTypes.h"

// This class decodes pixel hits from a color buffer and depth buffer
// extracting triangle IDs and edge types
class PixelDecoder
{
private:
	static void UnpackTriID(const uint8_t* pixel, VkFormat format, uint32_t& outTriID, EdgeType& outEdgeType);

public:
	PixelDecoder() = default;
	~PixelDecoder() = default;

    static std::vector<PixelHit> Decode(const uint8_t* colorData, const float* depthData, uint32_t width, uint32_t height, VkFormat colorFormat);    
    static bool DecodePixel(const uint8_t* colourData, const float* depthData, uint32_t width, uint32_t x, uint32_t y, VkFormat colorFormat, PixelHit& outHit);
};