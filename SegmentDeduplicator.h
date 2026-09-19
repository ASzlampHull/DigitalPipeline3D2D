#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "FeatureReadbackTypes.h"

class SegmentDeduplicator
{
public:
    struct Config {
        float positionEpsilon = 1e-4f;
        bool  keepHigherPriority = true;
    };

private:
	// Quantised position for deduplication, using integer coordinates to avoid floating point issues
    struct QuantisedPos {
        int32_t x, y, z;
		bool operator==(const QuantisedPos& o) const { // Checks if two QuantisedPos are equal
            return x == o.x && y == o.y && z == o.z;
        }
    };

	// Hash function for QuantisedPos to be used in unordered_map
	// This allows QuantisedPos to be used as a key in hash-based containers like std::unordered_map.
    struct QuantisedPosHash {
		size_t operator()(const QuantisedPos& p) const { // Checks if two QuantisedPos are equal
            return std::hash<int32_t>()(p.x) ^
                (std::hash<int32_t>()(p.y) << 1) ^
                (std::hash<int32_t>()(p.z) << 2);
        }
    };

    // Lower number means higher priority
    int EdgeTypePriority(EdgeType t) const {
        switch (t) {
        case EdgeType::SILHOUETTE: return 0;
        case EdgeType::BOUNDARY:   return 1;
        case EdgeType::CONTOUR:    return 2;
        case EdgeType::CREASE:     return 3;
        default:                   return 4;
        }
    }

    Config config;

    QuantisedPos Quantise(const glm::vec3& p) const;

public:
    SegmentDeduplicator() = default;
    ~SegmentDeduplicator() = default;

    void AddConfig(const Config& config_) { config = config_; }    

    std::vector<FeatureSegment> Deduplicate(const std::vector<FeatureSegment>& segments) const;
};