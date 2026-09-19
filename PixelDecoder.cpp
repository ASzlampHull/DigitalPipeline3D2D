#include "PixelDecoder.h"

// Unpack the triangle ID and edge type from a pixel's RGBA values
void PixelDecoder::UnpackTriID(const uint8_t* pixel, VkFormat format, uint32_t& outTriID, EdgeType& outEdgeType)
{
    // For R8G8B8A8_UNORM, bytes are [R, G, B, A]
    // triID is packed as: R = bits[0:7], G = bits[8:15], B = bits[16:23]
    // edgeType is packed into A as: edgeType / 3.0 * 255

    uint32_t r = pixel[0];
    uint32_t g = pixel[1];
    uint32_t b = pixel[2];
    uint32_t a = pixel[3];

    outTriID = r | (g << 8) | (b << 16);

	// Decode edgeType from alpha channel
    uint32_t edgeType = static_cast<uint32_t>((a / 255.0f) * 3.0f + 0.5f);
    if (edgeType > 3) edgeType = 3;
    outEdgeType = static_cast<EdgeType>(edgeType);
}

// Decode the entire mapped color buffer into pixel hits
std::vector<PixelHit> PixelDecoder::Decode(const uint8_t* colorData, const float* depthData, uint32_t width, uint32_t height, VkFormat colorFormat)
{
    std::vector<PixelHit> hits;
	hits.reserve(width * height / 8); // Reserve space for hits assuming not all pixels will be hits

    // Assume 4 bytes per pixel for R8G8B8A8_UNORM
    const uint32_t bytesPerPixel = 4;

    for (uint32_t y = 0; y < height; y++) {
        const float* depthRow = depthData + y * width;
        const uint8_t* colorRow = colorData + y * width * bytesPerPixel;

        for (uint32_t x = 0; x < width; x++) {
            const uint8_t* pixel = colorRow + x * bytesPerPixel;

            // Depth rejection: skip pixels with no geometry
            float depth = depthRow[x];
            if (depth >= NO_DEPTH_CONST) continue;

            // Unpack triID and edgeType from RGBA
            uint32_t triID;
            EdgeType edgeType;
            UnpackTriID(pixel, colorFormat, triID, edgeType);

            // Sentinel rejection: skip "no feature" pixels
            if (triID == NO_FEATURE_CONST) continue;
            if (edgeType == EdgeType::NONE) continue;

            PixelHit hit;
            hit.x = x;
            hit.y = y;
            hit.depth = depth;
            hit.triID = triID;
            hit.edgeType = edgeType;
            hits.push_back(hit);
        }
    }

    return hits;
}

// Decode a single pixel (used for testing)
bool PixelDecoder::DecodePixel(const uint8_t* colourData, const float* depthData, uint32_t width, uint32_t x, uint32_t y, VkFormat colorFormat, PixelHit& outHit)
{
	return false;
}
