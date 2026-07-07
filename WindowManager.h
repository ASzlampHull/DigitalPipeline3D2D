#pragma once
#include <GLFW/glfw3.h>
#include "InputManager.h"
#include "ConfigLoader.h"


class WindowManager final {
private:
	GLFWwindow* window = nullptr;
	const char* title = nullptr;
	int width = 0;
	int height = 0;
	bool framebufferResized = false;

	static void framebufferResizeCallback(GLFWwindow* window_, int width_, int height_) {
		const auto app = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window_));
		app->framebufferResized = true;
		app->width = width_;
		app->height = height_;
	}

public:
	WindowManager() = default;
	explicit WindowManager(const ConfigData& configData)
		: window(nullptr),
		title(configData.title.c_str()),
		width(configData.resolution.width),
		height(configData.resolution.height),
		framebufferResized(false)
	{
		CreateWindow();
	}
	WindowManager(const WindowManager& other) = default;
	WindowManager& operator=(const WindowManager& rhs)
	{
		if (this != &rhs) {
			window = rhs.window;
			title = rhs.title;
			width = rhs.width;
			height = rhs.height;
			framebufferResized = rhs.framebufferResized;
		}
		return *this;
	}
	GLFWwindow* GetGLFWwindow() const { return window; };

	void CreateWindow();
	const void Update(const InputManager& input) const;
	const bool* GetFramebufferResizedPtr() { return &framebufferResized; };
};