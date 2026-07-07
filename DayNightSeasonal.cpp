#include "DayNightSeasonal.h"

void DayNightSeasonal::AdvanceTimeOfDay()
{
	currentDayNightDuration -= deltaTime;
	currentSeasonDuration -= deltaTime;
}

void DayNightSeasonal::AdvanceSunLightSettings()
{
	const float sunRotationDirection = 5.0f;
	const float right = 6.5f; // Starting from three quarter rotation
	const float left = 3.3f; // Ending at one quarter rotation
	const float lightAmplifier = 0.001f * daySpeed;

	const float percent = 1.0f - (currentDayNightDuration / maxDayNightDuration);
	const float sunAngle = right + percent * (left - right);
	
	sunLightSettings.direction = glm::vec3(
		sunRotationDirection * std::cos(sunAngle),
		sunRotationDirection * std::sin(sunAngle),
		0.0f
	);
	const float halfPercentage = 0.5f;
	const float maxLightIntensity = 2.0f;
	const float minLightIntensity = 0.1f;
	if (currentTimeOfDay == TimeOfDay::DAY) {
		sunLightSettings.color = glm::vec3(1.0f, 0.956f, 0.839f);
		if (percent < halfPercentage && sunLightSettings.intensity < maxLightIntensity)
			sunLightSettings.intensity += lightAmplifier;
		else if (sunLightSettings.intensity > minLightIntensity)
			sunLightSettings.intensity -= lightAmplifier;
	}
	else if (currentTimeOfDay == TimeOfDay::NIGHT) {
		sunLightSettings.color = glm::vec3(0.4f, 0.4f, 0.5f);
		if (percent < halfPercentage && sunLightSettings.intensity > minLightIntensity)
			sunLightSettings.intensity -= lightAmplifier;
		else if (sunLightSettings.intensity < maxLightIntensity)
			sunLightSettings.intensity += lightAmplifier;
	}
}

const void DayNightSeasonal::AdvanceSunMoonPositions(Model& sunModel, Model& moonModel) const
{
	const float sunDistance = 360.0f;
	const float right = 6.5f;
	const float left = 3.3f;
	Transformations sunT = sunModel.GetTransformations();
	Transformations moonT = moonModel.GetTransformations();

	const float percent = 1.0f - (currentDayNightDuration / maxDayNightDuration);
	const float sunAngle = right + percent * (left - right);
	glm::vec3 sunPosition = {};
	glm::vec3 moonPosition = {};

	if (currentTimeOfDay == TimeOfDay::DAY) {
		sunPosition = glm::vec3(
			sunDistance * std::cos(sunAngle),
			sunDistance * std::sin(sunAngle),
			0.0f
		);
		moonPosition = glm::vec3(
			-sunDistance * std::cos(sunAngle),
			-sunDistance * std::sin(sunAngle),
			0.0f
		);
	}
	else if (currentTimeOfDay == TimeOfDay::NIGHT) {
		sunPosition = glm::vec3(
			-sunDistance * std::cos(sunAngle),
			-sunDistance * std::sin(sunAngle),
			0.0f
		);
		moonPosition = glm::vec3(
			sunDistance * std::cos(sunAngle),
			sunDistance * std::sin(sunAngle),
			0.0f
		);
	}
	sunT.position = sunPosition;
	sunModel.SetTransformations(sunT);
	moonT.position = moonPosition;
	moonModel.SetTransformations(moonT);
}

void DayNightSeasonal::UpdateDayNightSeason()
{
	if (currentDayNightDuration <= 0.0f) {
		if (currentTimeOfDay == TimeOfDay::DAY) {
			currentTimeOfDay = TimeOfDay::NIGHT;
			currentSeason = Season::WET;
		}
		else {
			currentTimeOfDay = TimeOfDay::DAY;
			currentSeason = Season::DRY;
		}
		SetDayNightDurations();
		if (currentSeason == Season::NONE)
			SetSeasonDurations();
	}
}

void DayNightSeasonal::Update(float deltaTime_, const InputManager& input, Model& sunModel, Model& moonModel)
{
	deltaTime = deltaTime_ * daySpeed;
	timeAccumulator += deltaTime;

	if (input.IsCommandDown(Commands::TimeSpeedUp)) { daySpeed += 0.01f; }
	if (input.IsCommandDown(Commands::TimeSlowDown)) { daySpeed -= 0.01f; }
	if (input.IsCommandDown(Commands::ParticlesOnDemand)) { currentSeason = Season::DRY; currentSeasonDuration = seasonalDuration.drySeasonDuration; }
	if (input.IsCommandDown(Commands::ApplicationReset)) { InitialiseDayNightSeasonal(); }

	if (daySpeed < 0.1f) daySpeed = 0.1f;

	AdvanceTimeOfDay();
	UpdateDayNightSeason();
	AdvanceSunLightSettings();
	AdvanceSunMoonPositions(sunModel, moonModel);
}

void DayNightSeasonal::InitialiseDayNightSeasonal()
{
	currentTimeOfDay = TimeOfDay::DAY;
	currentSeason = Season::DRY;
	SetDayNightDurations();
	SetSeasonDurations();
}

void DayNightSeasonal::SetDayNightDurations()
{
	switch (currentTimeOfDay) {
	case TimeOfDay::DAY:
		currentDayNightDuration = seasonalDuration.dayDuration;
		break;
	case TimeOfDay::NIGHT:
		currentDayNightDuration = seasonalDuration.nightDuration;
		break;
	default:
		__debugbreak(); // Should not reach here
		break;
	}
	maxDayNightDuration = currentDayNightDuration;
}

void DayNightSeasonal::SetSeasonDurations()
{
	switch (currentSeason) {
	case Season::DRY:
		currentSeasonDuration = seasonalDuration.drySeasonDuration;
		break;
	case Season::SNOW:
		currentSeasonDuration = seasonalDuration.snowSeasonDuration;
		break;
	case Season::WET:
		currentSeasonDuration = seasonalDuration.wetSeasonDuration;
		break;
	default:
		__debugbreak(); // Should not reach here
		break;
	}
	maxSeasonDuration = currentSeasonDuration;
}