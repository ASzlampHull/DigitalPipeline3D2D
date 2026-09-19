#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "FeatureReadbackTypes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Class for validating feature segments based on criteria such as length, view dot product, and in within bounds
class FeatureSegmentValidator
{
public:
    struct Config {
        float minWorldLength = 1e-4f;
        float maxWorldLength = 1000.0f;
        float minViewDot = -1.0f;
        float maxViewDot = 1.0f;
        bool  rejectNotANumber = true;
        bool  rejectOutOfBounds = true;
        glm::vec3 boundsMin{ -1000.0f };
        glm::vec3 boundsMax{ 1000.0f };
    };

private:
    Config config;

public:
    FeatureSegmentValidator() = default;
    ~FeatureSegmentValidator() = default;

    void AddConfig(const Config& config_) { config = config_; }

    std::vector<FeatureSegment> Filter(const std::vector<FeatureSegment>& segments) const;
    bool IsValid(const FeatureSegment& seg) const;
};