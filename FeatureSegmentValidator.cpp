#include "FeatureSegmentValidator.h"

// Filters a list of feature segments returning only those that are valid according to the current configuration.
std::vector<FeatureSegment> FeatureSegmentValidator::Filter(const std::vector<FeatureSegment>& segments) const
{
    std::vector<FeatureSegment> result;
    result.reserve(segments.size());
    for (const auto& seg : segments) {
        if (IsValid(seg)) result.push_back(seg);
    }
    return result;
}

// Checks if a feature segment is valid according to the current configuration.
bool FeatureSegmentValidator::IsValid(const FeatureSegment& seg) const
{
    // Edge check
    if (seg.edgeType == EdgeType::NONE) return false;
    if (static_cast<uint32_t>(seg.edgeType) >=
        static_cast<uint32_t>(EdgeType::COUNT)) return false;

    // Not a number check
    if (config.rejectNotANumber) {
        auto isNaN = [](const glm::vec3& v) { return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z); };
        if (isNaN(seg.startPos) || isNaN(seg.endPos)) return false;
    }

    // Length check
    float len = glm::distance(seg.startPos, seg.endPos);
    if (len < config.minWorldLength) return false;
    if (len > config.maxWorldLength) return false;

    // View Dot Product check
    if (seg.viewDot < config.minViewDot ||
        seg.viewDot > config.maxViewDot) return false;

    // Bounds check
    if (config.rejectOutOfBounds) {
        auto inBounds = [&](const glm::vec3& p) {
            return p.x >= config.boundsMin.x && p.x <= config.boundsMax.x &&
                p.y >= config.boundsMin.y && p.y <= config.boundsMax.y &&
                p.z >= config.boundsMin.z && p.z <= config.boundsMax.z;
            };
        if (!inBounds(seg.startPos) || !inBounds(seg.endPos)) return false;
    }

    return true;
}
