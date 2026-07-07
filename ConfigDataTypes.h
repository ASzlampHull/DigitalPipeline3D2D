#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Resolution {
	int width = 0;
	int height = 0;
};

struct ModelStats {
	std::string name = "Unnamed Model";
	std::string modelPath = "";
	std::string materialPath = "";
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
};

struct DirectionalLight {
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	float intensity = 1.0f;
};

struct SunMoonDirections {
	glm::vec3 sunDirection = glm::vec3(0.0f);
	glm::vec3 moonDirection = glm::vec3(0.0f);
};

struct SeasonalTimings {
	float dayDuration = 0.0f;
	float nightDuration = 0.0f;
	float drySeasonDuration = 0.0f;
	float snowSeasonDuration = 0.0f;
	float wetSeasonDuration = 0.0f;
};

struct ConfigData {
	std::unordered_map<std::string, ModelStats> models = {};
	DirectionalLight directionalLight = {};
	SunMoonDirections sunMoonDirections = {};
	SeasonalTimings seasonalTimings = {};
	Resolution resolution = {};
	std::string title = "Application";
};
