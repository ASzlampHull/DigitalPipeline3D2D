#include "HelloTriangleApplication.h"

void HelloTriangleApplication::InitConfiguration()
{
    configLoader = ConfigLoader("Configuration.ini");
    windowManager = WindowManager(configLoader.GetConfigData());
    inputManager = InputManager(windowManager.GetGLFWwindow());
    camera = Camera();
    renderer.InitRenderer(configLoader.GetConfigData(), windowManager.GetGLFWwindow(), camera.GetCurrentCamera());
}

void HelloTriangleApplication::Update()
{
    const float deltaTime = CalculateDeltaTime();

    inputManager.Update();
    camera.Update(deltaTime, inputManager);
    windowManager.Update(inputManager);
    renderer.Update(inputManager, camera.GetCurrentCamera(), deltaTime, windowManager.GetFramebufferResizedPtr());
}

void HelloTriangleApplication::MainLoop()
{
    while (!glfwWindowShouldClose(windowManager.GetGLFWwindow())) {
        glfwPollEvents();
        Update();
    }

    vkDeviceWaitIdle(renderer.GetCoreVulkan().device);
}

const float HelloTriangleApplication::CalculateDeltaTime() const
{
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<float> delta = currentTime - lastTime;
    lastTime = currentTime;

    return delta.count();
}
