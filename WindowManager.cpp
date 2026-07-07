#include "WindowManager.h"

void WindowManager::CreateWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

const void WindowManager::Update(const InputManager& input) const {
	if (input.IsCommandPressed(Commands::ApplicationQuit)) glfwSetWindowShouldClose(window, GLFW_TRUE);
}