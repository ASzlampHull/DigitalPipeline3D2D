#pragma once
#include <array>
#include <vector>
#include <optional>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 eyePos;
    alignas(16) glm::vec3 sunDirection;
    alignas(16) glm::vec3 sunLightColor;
    alignas(4) float time;
};

struct ModelPushConstants {
    alignas(16) glm::mat4 modelMatrix;
    alignas(16) glm::vec3 ambientColor; // Ka
    alignas(16) glm::vec3 diffuseColor; // Kd
    alignas(16) glm::vec3 specularColor; // Ks
    alignas(16) glm::vec3 emissionColor; // Ke
    alignas(4) float shininess; // Ns
    alignas(4) float refractionIndex; // Ni
    alignas(4) float transparency; // d
    alignas(1) bool isVertexShaded;
    alignas(1) bool isBumpHeight;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec2 texCoord;
    glm::vec3 tangent;
    glm::vec3 binormal;
    unsigned int triangleID;

    static void getBindingDescription(VkVertexInputBindingDescription& bindingDescription_) {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        bindingDescription_ = bindingDescription;
    }

    static void getAttributeDescriptions(std::array<VkVertexInputAttributeDescription, 7>& attributeDescriptions_) {
        std::array<VkVertexInputAttributeDescription, 7> attributeDescriptions{};
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);
        
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
         
        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, normal);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, texCoord);
        
        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(Vertex, tangent);
        
        attributeDescriptions[5].binding = 0;
        attributeDescriptions[5].location = 5;
        attributeDescriptions[5].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[5].offset = offsetof(Vertex, binormal);

        attributeDescriptions[6].binding = 0;
        attributeDescriptions[6].location = 6;
        attributeDescriptions[6].format = VK_FORMAT_R32_UINT;
        attributeDescriptions[6].offset = offsetof(Vertex, triangleID);

        attributeDescriptions_ = attributeDescriptions;
    }
};

using MeshObject = std::vector<Vertex>;