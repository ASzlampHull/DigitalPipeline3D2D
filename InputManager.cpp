#include "InputManager.h"

// Check if a key is currently being held down
void InputManager::Update() {
	previous = current;

	HandleCameraDirectionalMovement();	
	HandleCameraVerticals();	
	HandleCameraRotations();
	HandleCameraSwitching();
	HandleApplicationCommands();
	HandleParticlesOnDemand();
	HandleSimulationSpeed();
	HandleIMGUI();
}

void InputManager::HandleSimulationSpeed()
{
	current[Commands::TimeSpeedUp] = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	current[Commands::TimeSlowDown] = (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS));
}

void InputManager::HandleParticlesOnDemand()
{
	current[Commands::ParticlesOnDemand] = (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS);
}

void InputManager::HandleApplicationCommands()
{
	current[Commands::ApplicationReset] = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
	current[Commands::ApplicationQuit] = (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS);
}

void InputManager::HandleCameraSwitching()
{
	current[Commands::CameraSwitch1] = (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS);
	current[Commands::CameraSwitch2] = (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS);
	current[Commands::CameraSwitch3] = (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS);
}

void InputManager::HandleCameraRotations()
{
	current[Commands::CameraRotateUp] = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS));
	current[Commands::CameraRotateDown] = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS));
	current[Commands::CameraRotateLeft] = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && !(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS));
	current[Commands::CameraRotateRight] = (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && !(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS));
}

void InputManager::HandleCameraVerticals()
{
	current[Commands::CameraMoveUp] = (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS);
	current[Commands::CameraMoveDown] = (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS);
}

void InputManager::HandleCameraDirectionalMovement()
{
	current[Commands::CameraMoveForward] = (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	current[Commands::CameraMoveBackward] = (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	current[Commands::CameraMoveLeft] = (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
	current[Commands::CameraMoveRight] = (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS);
}

void InputManager::HandleIMGUI()
{
	current[Commands::DisplayIMGUI] = (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS);
}
