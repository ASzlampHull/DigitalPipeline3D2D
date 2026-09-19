#include "PixelSegmentMerger.h"

// Group pixels by triID and edgeType
std::unordered_map<uint64_t, std::vector<const PixelHit*>> PixelSegmentMerger::GroupByKey(const std::vector<PixelHit>& pixelHits) const
{
    std::unordered_map<uint64_t, std::vector<const PixelHit*>> pixelHitsGroups;

    for (const auto& hit : pixelHits) {
        // Composite key: 24-bit triID | 4-bit edgeType | 4 bits padding
        uint64_t key = (static_cast<uint64_t>(hit.triID) << 8) | (static_cast<uint64_t>(hit.edgeType) & 0xF);
        pixelHitsGroups[key].push_back(&hit);
    }

    return pixelHitsGroups;
}


// Finds the connected components of a group of pixels
std::vector<std::vector<const PixelHit*>> PixelSegmentMerger::FindConnectedComponents(const std::vector<const PixelHit*>& pixelHitsSource) const
{
    std::unordered_map<uint32_t, const PixelHit*> pixelGrid;
    pixelGrid.reserve(pixelHitsSource.size() * 2);

    for (const auto* hit : pixelHitsSource) {
        uint32_t gridKey = (hit->y << 16) | (hit->x & 0xFFFF);
        pixelGrid[gridKey] = hit;
    }

	std::unordered_set<const PixelHit*> pixelHitsVisited; // Set to track visited pixels
	std::vector<std::vector<const PixelHit*>> pixelHitsComponents; // Vector to store connected components

    for (const auto* startPixel : pixelHitsSource) {
        if (pixelHitsVisited.count(startPixel)) continue;

        std::vector<const PixelHit*> pixelHitComponent;
        std::queue<const PixelHit*> nextPixelHit;
        nextPixelHit.push(startPixel);
        pixelHitsVisited.insert(startPixel);

        while (!nextPixelHit.empty()) {
            const PixelHit* current = nextPixelHit.front();
            nextPixelHit.pop();
            pixelHitComponent.push_back(current);

            // Check 8-connected neighbours within maxGroupRadius
            int radius = static_cast<int>(std::ceil(config.maxGroupRadius));
            for (int offsetY = -radius; offsetY <= radius; offsetY++) {
                for (int offsetX = -radius; offsetX <= radius; offsetX++) {
                    if (offsetX == 0 && offsetY == 0) continue;

                    int neighbourPixelX = static_cast<int>(current->x) + offsetX;
                    int neighbourPixelY = static_cast<int>(current->y) + offsetY;
                    if (neighbourPixelX < 0 || neighbourPixelY < 0 || neighbourPixelX > 0xFFFF || neighbourPixelY > 0xFFFF)
                        continue;

                    // Check distance constraint
                    if (float(offsetX * offsetX + offsetY * offsetY) >
                        config.maxGroupRadius * config.maxGroupRadius)
                        continue;

                    uint32_t neighbourPixelKey = (static_cast<uint32_t>(neighbourPixelY) << 16) |
                        (static_cast<uint32_t>(neighbourPixelX) & 0xFFFF);
                    auto pixelGridIter = pixelGrid.find(neighbourPixelKey);
                    if (pixelGridIter == pixelGrid.end()) continue;

                    const PixelHit* neighbor = pixelGridIter->second;
                    if (pixelHitsVisited.count(neighbor)) continue;

                    pixelHitsVisited.insert(neighbor);
                    nextPixelHit.push(neighbor);
                }
            }
        }

        pixelHitsComponents.push_back(std::move(pixelHitComponent));
    }

    return pixelHitsComponents;
}

// For a connected component, find the two endpoints
// (pixels that are furthest apart along the segment direction)
bool PixelSegmentMerger::FindEndpoints(const std::vector<const PixelHit*>& pixelHitComponent, const PixelHit*& pixelHitOutStart, const PixelHit*& pixelHitOutEnd) const
{
    if (pixelHitComponent.size() < 2) return false;

    glm::vec2 axis = ComputePrincipalAxis(pixelHitComponent);

    float minProjection = std::numeric_limits<float>::max();
    float maxProjection = -std::numeric_limits<float>::max();

    for (const auto* hit : pixelHitComponent) {
        glm::vec2 projection(hit->x, hit->y);
        float projectionDot = glm::dot(projection, axis);
        if (projectionDot < minProjection) { minProjection = projectionDot; pixelHitOutStart = hit; }
        if (projectionDot > maxProjection) { maxProjection = projectionDot; pixelHitOutEnd = hit; }
    }

    return pixelHitOutStart != nullptr && pixelHitOutEnd != nullptr;
}

// Find which edge of the triangle this segment lies on
uint32_t PixelSegmentMerger::FindEdgeIndex(uint32_t triID, const glm::vec3& startWorldPos, const glm::vec3& endWorldPos) const
{
    uint32_t indexA = (indices)[triID * 3 + 0];
    uint32_t indexB = (indices)[triID * 3 + 1];
    uint32_t indexC = (indices)[triID * 3 + 2];

    glm::vec3 posA = (vertices)[indexA];
    glm::vec3 posB = (vertices)[indexB];
    glm::vec3 posC = (vertices)[indexC];

    glm::vec3 midpoint = 0.5f * (startWorldPos + endWorldPos);

    // Distance from midpoint to each triangle edge; closest edge wins
    auto pointToSegmentDistance = [](const glm::vec3& midpoint, const glm::vec3& posA, const glm::vec3& posB) {
            glm::vec3 posAB = posB - posA;
            float closestToMidpoint = glm::clamp(glm::dot(midpoint - posA, posAB) / glm::dot(posAB, posAB), 0.0f, 1.0f);
            return glm::distance(midpoint, posA + closestToMidpoint * posAB);
        };

    float edgeA = pointToSegmentDistance(midpoint, posB, posC);
    float edgeB = pointToSegmentDistance(midpoint, posC, posA);
    float edgeC = pointToSegmentDistance(midpoint, posA, posB);

    if (edgeA <= edgeB && edgeA <= edgeC) return 0;
    if (edgeB <= edgeC) return 1;
    return 2;
}

// This function computes the principal axis of a set of pixels using PCA (Principal Component Analysis)
glm::vec2 PixelSegmentMerger::ComputePrincipalAxis(const std::vector<const PixelHit*>& pixelHitComponent) const
{
    // Compute mean
    glm::vec2 meanAverage(0.0f);
    for (const auto* hit : pixelHitComponent) {
        meanAverage += glm::vec2(hit->x, hit->y);
    }
    meanAverage /= float(pixelHitComponent.size());

    // Compute covariance matrix
    float matrixXX = 0.0f, matrixXY = 0.0f, matrixYY = 0.0f;
    for (const auto* hit : pixelHitComponent) {
        glm::vec2 offset = glm::vec2(hit->x, hit->y) - meanAverage;
        matrixXX += offset.x * offset.x;
        matrixXY += offset.x * offset.y;
        matrixYY += offset.y * offset.y;
    }
    float pixelHitComponentSize = float(pixelHitComponent.size());
    matrixXX /= pixelHitComponentSize; matrixXY /= pixelHitComponentSize; matrixYY /= pixelHitComponentSize;

	// Works out the principal axis direction from the covariance matrix
    float theta = 0.5f * std::atan2(2.0f * matrixXY, matrixXX - matrixYY);
    return glm::vec2(std::cos(theta), std::sin(theta));
}

// Build a FeatureSegment from two endpoint pixels
FeatureSegment PixelSegmentMerger::BuildSegment(const PixelHit* pixelHitStart, const PixelHit* pixelHitEnd, const std::function<glm::vec3(uint32_t)>& triangleNormalLookup, const glm::vec3& eyePosition, const glm::vec3& colourSampler) const
{
    FeatureSegment seg;
    seg.startPos = pixelHitStart->worldPos;
    seg.endPos = pixelHitEnd->worldPos;
    seg.triID = pixelHitStart->triID;
    seg.edgeType = pixelHitStart->edgeType;
    seg.depth = 0.5f * (pixelHitStart->depth + pixelHitEnd->depth);

    glm::vec3 normal = triangleNormalLookup(seg.triID);
    glm::vec3 viewDir = glm::normalize(eyePosition - seg.startPos);
    seg.viewDot = glm::dot(normal, viewDir);

    glm::vec3 midPoint = 0.5f * (seg.startPos + seg.endPos);
    seg.colour = colourSampler;

    seg.edgeIndex = FindEdgeIndex(seg.triID, seg.startPos, seg.endPos);

    return seg;
}

// This function merges pixel hits into feature segments, using the configuration parameters and triangle data provided.
std::vector<FeatureSegment> PixelSegmentMerger::Merge(const std::vector<PixelHit>& pixelHits, uint32_t width, uint32_t height, const std::function<glm::vec3(uint32_t)>& triangleNormalLookup, const glm::vec3& eyePosition, const glm::vec3& colorSampler)
{
    std::vector<FeatureSegment> segments;
    segments.reserve(pixelHits.size() / 8);

    auto pixelHitGroups = GroupByKey(pixelHits);

    // Each group finds connected components
    for (auto& [key, group] : pixelHitGroups) {
        if (group.size() < 2) continue;

        std::vector<std::vector<const PixelHit*>> components;
        if (config.useConnectedComponents) {
            components = FindConnectedComponents(group);
        }
        else {
            components.push_back(group);
        }

        // Each component find endpoints and build segment
        for (auto& component : components) {
            if (component.size() < 2) continue;

            const PixelHit* start = nullptr;
            const PixelHit* end = nullptr;
            if (!FindEndpoints(component, start, end)) continue;

            // Screen-space length check
            float screenDist = std::sqrt(
                float((start->x - end->x) * (start->x - end->x) +
                    (start->y - end->y) * (start->y - end->y)));
            if (screenDist < config.minScreenLength) continue;

            FeatureSegment seg = BuildSegment(start, end, triangleNormalLookup, eyePosition, colorSampler);

            if (seg.IsValid() &&
                glm::distance(seg.startPos, seg.endPos) >= config.minWorldLength) {
                segments.push_back(seg);
            }
        }
    }

    return segments;
}