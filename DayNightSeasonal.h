#pragma once
#include "ConfigLoader.h"
#include "InputManager.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>


struct SeasonalDuration {
	float dayDuration;
	float nightDuration;
	float drySeasonDuration;
	float snowSeasonDuration;
	float wetSeasonDuration;
};

struct SunLightSettings {
	glm::vec3 direction;
	glm::vec3 color;
	float intensity;
};

struct SMDirections {
	glm::vec3 sunDirection;
	glm::vec3 moonDirection;
};

enum class TimeOfDay {
	DAY,
	NIGHT
};
enum class Season {
	NONE,
	DRY,
	SNOW,
	WET
};

class DayNightSeasonal final{
private:

	TimeOfDay currentTimeOfDay = TimeOfDay::DAY;
	Season currentSeason = Season::DRY;
	SeasonalDuration seasonalDuration = {};
	SunLightSettings sunLightSettings = {};
	SMDirections sunMoonDirections = {};
	float currentDayNightDuration = 0.0f;
	float currentSeasonDuration = 0.0f;
	float maxDayNightDuration = 0.0f;
	float maxSeasonDuration = 0.0f;
	float deltaTime = 0.0f;
	float timeAccumulator = 0.0f;
	float daySpeed = 1.0f;

	void AdvanceTimeOfDay();
	void AdvanceSunLightSettings();
	const void AdvanceSunMoonPositions(Model& sunModel, Model& moonModel) const;
	void UpdateDayNightSeason();
	void SetDayNightDurations();
	void SetSeasonDurations();

public:
	DayNightSeasonal() = default;
	~DayNightSeasonal() = default;

	explicit DayNightSeasonal(const ConfigData& configData)
		:	seasonalDuration{
				configData.seasonalTimings.dayDuration,
				configData.seasonalTimings.nightDuration,
				configData.seasonalTimings.drySeasonDuration,
				configData.seasonalTimings.snowSeasonDuration,
				configData.seasonalTimings.wetSeasonDuration },
			sunLightSettings{
				configData.directionalLight.direction,
				configData.directionalLight.color,
				configData.directionalLight.intensity },
			sunMoonDirections{
				configData.sunMoonDirections.sunDirection,
				configData.sunMoonDirections.moonDirection }
	{
		InitialiseDayNightSeasonal();
	}

	DayNightSeasonal& operator=(const DayNightSeasonal& rhs)
	{
		if (this != &rhs) {
			currentTimeOfDay = rhs.currentTimeOfDay;
			currentSeason = rhs.currentSeason;
			seasonalDuration = rhs.seasonalDuration;
			sunLightSettings = rhs.sunLightSettings;
			sunMoonDirections = rhs.sunMoonDirections;
			currentDayNightDuration = rhs.currentDayNightDuration;
			currentSeasonDuration = rhs.currentSeasonDuration;
			maxDayNightDuration = rhs.maxDayNightDuration;
			maxSeasonDuration = rhs.maxSeasonDuration;
			deltaTime = rhs.deltaTime;
			timeAccumulator = rhs.timeAccumulator;
			daySpeed = rhs.daySpeed;
		}
		return *this;
	}

	void Update(float deltaTime, const InputManager& input, Model& sunModel, Model& moonModel);

	void InitialiseDayNightSeasonal();

	//TimeOfDay GetCurrentTimeOfDay() const { return currentTimeOfDay; }
	Season GetCurrentSeason() const { return currentSeason; }
	const SunLightSettings& GetSunLightSettings() const { return sunLightSettings; }
};