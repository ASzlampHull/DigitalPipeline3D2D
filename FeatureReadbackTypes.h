#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

enum class EdgeType : uint32_t {
    SILHOUETTE = 0,
    CONTOUR = 1,
    CREASE = 2,
    BOUNDARY = 3,
    COUNT = 4,
    NONE = 0xFFFFFFFF
};

// Geometry representation of a single feature segment as decoded from the GPU
struct FeatureSegment {
    // Geometry (world space)
    glm::vec3 startPos;
    glm::vec3 endPos;

    // Identification
    uint32_t  triID = 0;
    uint32_t  edgeIndex = 0;
    EdgeType  edgeType = EdgeType::NONE;

	float viewDot = 0.0f; // Dot product of view direction and edge normal, for sorting
	float depth = 1.0f; // Depth of the segment

	glm::vec3 colour = glm::vec3(1.0f); // Colour for rendering

    bool IsValid() const {
        return edgeType != EdgeType::NONE &&
            glm::distance(startPos, endPos) > 1e-5f;
    }
};

// Representation of a single pixel hit, as decoded from the GPU
// Meaning a pixel that contains a feature edge
struct PixelHit {
    uint32_t x = 0;
    uint32_t y = 0;
    float    depth = 1.0f; // Used for depth sorting
    uint32_t triID = 0;
    EdgeType edgeType = EdgeType::NONE;
    glm::vec3 worldPos = glm::vec3(0.0f);
};

// Represents the configuration of the readback target (framebuffer) used for feature extraction
struct ReadbackTargetConfig {
    uint32_t width = 2048;
    uint32_t height = 2048;
    VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    uint32_t frameCount = 2;
};

// Represents a pixel that contains no feature edge
static constexpr uint32_t NO_FEATURE_CONST = 0xFFFFFF;
// Represents a pixel with a value for no depth
static constexpr float NO_DEPTH_CONST = 1.0f;

// Represents the path of a feature edge for drawing a line, as decoded from the GPU
struct FeaturePath {
    std::vector<glm::vec3> points;  // Point in a complete path 
    bool isClosed;                   // Is it a closed loop
    uint32_t triID;                  
    EdgeType type;                   

	// Function for calculating the total length of the path
    float totalLength() const {
        float len = 0;
        for (size_t i = 1; i < points.size(); i++) {
            len += glm::distance(points[i - 1], points[i]);
        }
        return len;
    }
};