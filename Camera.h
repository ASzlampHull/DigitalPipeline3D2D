#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <algorithm>
#include "InputManager.h"


struct CameraSettings {
	glm::mat4 viewMatrix;
	glm::vec3 eyePosition;
	glm::vec3 centrePosition;
	glm::vec3 upPosition;
	glm::vec3 front;
	glm::vec3 right;
	float yaw;
	float pitch;
};

class Camera final{
private:
	CameraSettings currentCamera;
	std::vector<CameraSettings> cameraSets;
	const float cameraSpeed = 20.0f;
	const float rotationSpeed = 30.0f;

	const void InitialiseCamera(CameraSettings& camera, const glm::vec3& eyePos, const glm::vec3& cenPos, const glm::vec3& upPos) const;
	void InintialiseCameraSet();
	void UpdateCameraVectors();
	void SwitchCamera(int cameraIndex);

public:
	Camera();
	~Camera() = default;

	void Update(float deltaTime, const InputManager& input);
	glm::mat4 GetViewMatrix() const { return currentCamera.viewMatrix; }
	const CameraSettings& GetCurrentCamera() const { return currentCamera; }

	Camera& operator=(const Camera& rhs)
	{
		if (this != &rhs) {
			this->currentCamera = rhs.currentCamera;
			this->cameraSets = rhs.cameraSets;
		}
		return *this;
	}
};