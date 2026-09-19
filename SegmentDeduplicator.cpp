#include "SegmentDeduplicator.h"

// Quantise a position into a quantised integer position based on the configured epsilon.
SegmentDeduplicator::QuantisedPos SegmentDeduplicator::Quantise(const glm::vec3& pos) const
{
    const float inverse = 1.0f / config.positionEpsilon;
    return {
        static_cast<int32_t>(std::round(pos.x * inverse)),
        static_cast<int32_t>(std::round(pos.y * inverse)),
        static_cast<int32_t>(std::round(pos.z * inverse))
    };
}

// Remove duplicate segments (same endpoints, different triID)
std::vector<FeatureSegment> SegmentDeduplicator::Deduplicate(const std::vector<FeatureSegment>& segments) const
{
	// Represents a unique edge defined by its two quantised endpoints.
    struct EdgeKey {
        QuantisedPos posA, posB;
		bool operator==(const EdgeKey& o) const { // Checks if two EdgeKey are equal
            return posA == o.posA && posB == o.posB;
        }
    };
	// Represents a hash function for EdgeKey to be used in unordered_map
    struct EdgeKeyHash {
        size_t operator()(const EdgeKey& edgeKey) const {
            QuantisedPosHash posHash;
			return posHash(edgeKey.posA) ^ (posHash(edgeKey.posB) << 1); // Checks if two EdgeKey are equal
        }
    };

	std::unordered_map<EdgeKey, size_t, EdgeKeyHash> seen; // Map to track seen edges and their corresponding index in the result vector
    std::vector<FeatureSegment> result;
    result.reserve(segments.size());

    for (const auto& seg : segments) {
        QuantisedPos quantisedPosA = Quantise(seg.startPos);
        QuantisedPos quantisedPosB = Quantise(seg.endPos);
                
        EdgeKey currentEdgeKey;
		// Sorting the quantised positions to ensure consistent ordering for the edge key
        if (quantisedPosA.x < quantisedPosB.x || (quantisedPosA.x == quantisedPosB.x && quantisedPosA.y < quantisedPosB.y) ||
            (quantisedPosA.x == quantisedPosB.x && quantisedPosA.y == quantisedPosB.y && quantisedPosA.z <= quantisedPosB.z)) {
            currentEdgeKey = { quantisedPosA, quantisedPosB };
        }
        else {
            currentEdgeKey = { quantisedPosB, quantisedPosA };
        }

		// Checks if the edge has already been seen
        auto keyIter = seen.find(currentEdgeKey);
        if (keyIter == seen.end()) {
            seen[currentEdgeKey] = result.size();
            result.push_back(seg);
        }
		// Checks if the current segment has a higher priority than the previously stored segment for the same edge
        else if (config.keepHigherPriority) {
            size_t indexResult = keyIter->second;
            if (EdgeTypePriority(seg.edgeType) <
                EdgeTypePriority(result[indexResult].edgeType)) {
                result[indexResult] = seg;
            }
        }
    }

    return result;
}