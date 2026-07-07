#pragma once

#include "Camera.h"
#include "InputManager.h"
#include "WindowManager.h"
#include "ConfigLoader.h"
#include "ModelLoader.h"

#include "VulkanExternals.h"
#include "Renderer.h"

#include "RenderDataTypes.h"
#include "VulkanDataTypes.h"

class HelloTriangleApplication final{
public:
    void run() {
        InitConfiguration();
        MainLoop();
        renderer.Cleanup();
    }

    HelloTriangleApplication() = default;
    ~HelloTriangleApplication() = default;

private:

    // Helper Classes - AS
    InputManager inputManager;
    ConfigLoader configLoader;

    // Vulkan Objects - AS
    Camera camera;
    WindowManager windowManager;
    Renderer renderer;

    void InitConfiguration();
    void Update();
    void MainLoop();
    const float CalculateDeltaTime() const;
};