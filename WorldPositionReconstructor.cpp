#include "WorldPositionReconstructor.h"

// Reconstruct world position for a pixel hit
// Converted to normalized device coordinates (NDC) and then transformed to world space using the inverse view-projection matrix.
glm::vec3 WorldPositionReconstructor::Reconstruct(const PixelHit& pixelHit, uint32_t width, uint32_t height) const
{
	const float epsilon = 1e-8f;

    float ndcX = (2.0f * (pixelHit.x + 0.5f)) / width - 1.0f;
    float ndcY = (2.0f * (pixelHit.y + 0.5f)) / height - 1.0f;

    // ndcY = -ndcY; // Flip in case the Y-axis is inverted

    float ndcZ = pixelHit.depth;

    // Clip space -> world space
    glm::vec4 clipPos(ndcX, ndcY, ndcZ, 1.0f);
    glm::vec4 worldPos = inverseViewProj * clipPos;

	// Persective divide to get world position
    if (std::abs(worldPos.w) > epsilon) {
        worldPos /= worldPos.w;
    }

    return glm::vec3(worldPos);
}

void WorldPositionReconstructor::ReconstructAll(std::vector<PixelHit>& hits, uint32_t width, uint32_t height) const
{
    for (auto& hit : hits) {
        hit.worldPos = Reconstruct(hit, width, height);
    }
}
