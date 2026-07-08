#include "Camera.h"

Camera::Camera() {
	InintialiseCameraSet();
}

const void Camera::InitialiseCamera(CameraSettings& camera, const glm::vec3& eyePos, const glm::vec3& cenPos, const glm::vec3& upPos) const {
	camera.eyePosition = eyePos;
	camera.centrePosition = cenPos;
	camera.upPosition = upPos;
	camera.front = glm::normalize(camera.centrePosition - camera.eyePosition);
	camera.right = glm::normalize(glm::cross(camera.front, camera.upPosition));
	
	camera.pitch = glm::degrees(asin(glm::clamp(camera.front.y, -1.0f, 1.0f)));
	camera.yaw = glm::degrees(atan2(camera.front.z, camera.front.x));

	camera.viewMatrix = glm::lookAt(camera.eyePosition, camera.centrePosition, camera.upPosition);
}

void Camera::InintialiseCameraSet() {
	glm::vec3 cameraEyePos = glm::vec3(0.0f, 0.0f, -10.0f);
	glm::vec3 cameraCentrePos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraUpPos = glm::vec3(0.0f, -1.0f, 0.0f);
	CameraSettings cam1;
	InitialiseCamera(cam1, cameraEyePos, cameraCentrePos, cameraUpPos);
	cameraSets.push_back(cam1);

	cameraEyePos = glm::vec3(6.0f, -2.5f, -10.0f);
	cameraCentrePos = glm::vec3(6.0f, -2.5f, 0.0f);
	cameraUpPos = glm::vec3(0.0f, -1.0f, 0.0f);
	CameraSettings cam2;
	InitialiseCamera(cam2, cameraEyePos, cameraCentrePos, cameraUpPos);
	cameraSets.push_back(cam2);

	cameraEyePos = glm::vec3(0.0f, -2.0f, -7.0f);
	cameraCentrePos = glm::vec3(0.0f, -2.0f, 0.0f);
	cameraUpPos = glm::vec3(0.0f, -1.0f, 0.0f);
	CameraSettings cam3;
	InitialiseCamera(cam3, cameraEyePos, cameraCentrePos, cameraUpPos);
	cameraSets.push_back(cam3);
	currentCamera = cameraSets[0];
}

void Camera::UpdateCameraVectors() {
	glm::vec3 front;
	front.x = cos(glm::radians(currentCamera.yaw)) * cos(glm::radians(currentCamera.pitch));
	front.y = sin(glm::radians(currentCamera.pitch));
	front.z = sin(glm::radians(currentCamera.yaw)) * cos(glm::radians(currentCamera.pitch));
	currentCamera.front = glm::normalize(front);
	currentCamera.right = glm::normalize(glm::cross(currentCamera.front, currentCamera.upPosition));
	currentCamera.centrePosition = currentCamera.eyePosition + currentCamera.front;
	currentCamera.viewMatrix = glm::lookAt(currentCamera.eyePosition, currentCamera.centrePosition, currentCamera.upPosition);
}

void Camera::Update(float deltaTime, const InputManager& input) {
	const float velocity = cameraSpeed * deltaTime;
	const float rotationVelocity = rotationSpeed * deltaTime;

	if (input.IsCommandDown(Commands::CameraMoveForward)) currentCamera.eyePosition += currentCamera.front * velocity;
	if (input.IsCommandDown(Commands::CameraMoveBackward)) currentCamera.eyePosition -= currentCamera.front * velocity;
	if (input.IsCommandDown(Commands::CameraMoveLeft)) currentCamera.eyePosition -= currentCamera.right * velocity;
	if (input.IsCommandDown(Commands::CameraMoveRight)) currentCamera.eyePosition += currentCamera.right * velocity;

	if (input.IsCommandDown(Commands::CameraMoveUp))    currentCamera.eyePosition += currentCamera.upPosition * velocity;
	if (input.IsCommandDown(Commands::CameraMoveDown))  currentCamera.eyePosition -= currentCamera.upPosition * velocity;

	if (input.IsCommandDown(Commands::CameraRotateLeft))  currentCamera.yaw += rotationVelocity;
	if (input.IsCommandDown(Commands::CameraRotateRight)) currentCamera.yaw -= rotationVelocity;
	if (input.IsCommandDown(Commands::CameraRotateUp))    currentCamera.pitch = std::clamp(currentCamera.pitch - rotationVelocity, -89.0f, 89.0f);
	if (input.IsCommandDown(Commands::CameraRotateDown))  currentCamera.pitch = std::clamp(currentCamera.pitch + rotationVelocity, -89.0f, 89.0f);

	if (input.IsCommandPressed(Commands::CameraSwitch1)) SwitchCamera(0);
	if (input.IsCommandPressed(Commands::CameraSwitch2)) SwitchCamera(1);
	if (input.IsCommandPressed(Commands::CameraSwitch3)) SwitchCamera(2);

	UpdateCameraVectors();
}

void Camera::SwitchCamera(int cameraIndex) {
	if (cameraIndex >= 0 && cameraIndex < static_cast<int>(cameraSets.size())) {
		currentCamera = cameraSets[cameraIndex];
	}
	else {
		assert(!"SwitchCamera called with invalid index");
	}
}