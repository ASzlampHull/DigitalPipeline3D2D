#pragma once
#include "VulkanDataTypes.h"
#include "VulkanExternals.h"
#include "FeatureReadbackTypes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// This class reconstructs world positions from pixel hits using the inverse of the view-projection matrix.
class WorldPositionReconstructor
{
private:
	glm::mat4 view{ 1.0f };
	glm::mat4 projection{ 1.0f };
	glm::mat4 inverseViewProj{ 1.0f };

public:
	WorldPositionReconstructor() = default;
	~WorldPositionReconstructor() = default;
	WorldPositionReconstructor(const glm::mat4& view_, const glm::mat4& projection_) : view(view_), projection(projection_) {
		inverseViewProj = glm::inverse(projection * view);
	}
	WorldPositionReconstructor(const WorldPositionReconstructor& other) = default;
	WorldPositionReconstructor& operator=(const WorldPositionReconstructor& rhs)
	{
		if (this != &rhs) {
			this->view = rhs.view;
			this->projection = rhs.projection;
			this->inverseViewProj = rhs.inverseViewProj;
		}
		return *this;
	}

	glm::vec3 Reconstruct(const PixelHit& hit, uint32_t width, uint32_t height) const;
	void ReconstructAll(std::vector<PixelHit>& hits, uint32_t width, uint32_t height) const;
};