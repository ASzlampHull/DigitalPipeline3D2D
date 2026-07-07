#pragma once

#include <GLFW/glfw3.h>
#include <unordered_map>

enum class Commands {
	CameraMoveForward,
	CameraMoveBackward,
	CameraMoveLeft,
	CameraMoveRight,
	CameraMoveUp,
	CameraMoveDown,
	CameraRotateUp,
	CameraRotateDown,
	CameraRotateLeft,
	CameraRotateRight,
	CameraSwitch1,
	CameraSwitch2,
	CameraSwitch3,
	ApplicationReset,
	ApplicationQuit,
	ParticlesOnDemand,
	TimeSpeedUp,
	TimeSlowDown,
	DisplayIMGUI
};

class InputManager final{
private:
	std::unordered_map<Commands, bool> current;
	std::unordered_map<Commands, bool> previous;
	GLFWwindow* window;

public:
	InputManager() = default;
	explicit InputManager(GLFWwindow* window_) : window(window_) {};
	~InputManager() = default;
	InputManager(const InputManager& rhs) = default;

    void Update();
	void HandleSimulationSpeed();
	void HandleParticlesOnDemand();
	void HandleApplicationCommands();
	void HandleCameraSwitching();
	void HandleCameraRotations();
	void HandleCameraVerticals();
	void HandleCameraDirectionalMovement();
	void HandleIMGUI();
	bool IsCommandDown(Commands key) const { return current.at(key); };
	bool IsCommandPressed(Commands key) const { return current.at(key) && !previous.at(key); };

	InputManager& operator=(const InputManager& rhs)
	{
		if (this != &rhs) {
			this->window = rhs.window;
			this->current = rhs.current;
			this->previous = rhs.previous;
		}
		return *this;
	}
};