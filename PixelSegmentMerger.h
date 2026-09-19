#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "FeatureReadbackTypes.h"
#include <unordered_set>
#include <queue>

// This class is responsible for merging pixel hits into feature segments.
class PixelSegmentMerger
{
public:
    struct Config {
        float minScreenLength = 2.0f;    // Minimum pixels for a segment
        float minWorldLength = 1e-4f;   // Minimum world length
        float maxGroupRadius = 4.0f;    // Max pixel distance for grouping
        bool  useConnectedComponents = true;
    };

private:
    std::vector<glm::vec3> vertices = {};
    std::vector<uint32_t> indices = {};
    Config config;

    std::unordered_map<uint64_t, std::vector<const PixelHit*>> GroupByKey(const std::vector<PixelHit>& hits) const;
    std::vector<std::vector<const PixelHit*>> FindConnectedComponents(const std::vector<const PixelHit*>& group) const;
    bool FindEndpoints(const std::vector<const PixelHit*>& component, const PixelHit*& outStart, const PixelHit*& outEnd) const;
    uint32_t FindEdgeIndex(uint32_t triID, const glm::vec3& startWorld, const glm::vec3& endWorld) const;
    glm::vec2 ComputePrincipalAxis(const std::vector<const PixelHit*>& component) const;
    FeatureSegment BuildSegment(const PixelHit* start, const PixelHit* end, const std::function<glm::vec3(uint32_t)>& triangleNormalLookup, const glm::vec3& eyePosition, const glm::vec3& colorSampler) const;

public:
    PixelSegmentMerger() = default;
    ~PixelSegmentMerger() = default;

	void AddConfig(const Config& config_) { config = config_; }

    std::vector<FeatureSegment> Merge(const std::vector<PixelHit>& hits, uint32_t width, uint32_t height, const std::function<glm::vec3(uint32_t)>& triangleNormalLookup, const glm::vec3& eyePosition, const glm::vec3& colourSampler);    

    // Accessors
    void SetTriangleData(const std::vector<glm::vec3> vertices_, const std::vector<uint32_t> indices_) {
        vertices = vertices_;
        indices = indices_;
    }
	const std::vector<glm::vec3> GetTriangleVertices() const { return vertices; }
	const std::vector<uint32_t> GetTriangleIndices() const { return indices; }
};