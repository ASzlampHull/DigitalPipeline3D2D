#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 eyePos;
    vec3 lightDir;
    vec3 lightColor;
    float time;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec3 ambientMat;
    vec3 diffuseMat;
    vec3 specularMat;
    vec3 emissiveMat;
    float shininess;
    float reflectivity;
    float opacity;
    bool isVertexShaded;
    bool isBumpHeight;
} pushConstants;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inTexCoord;
layout(location = 4) in vec3 inTangent;
layout(location = 5) in vec3 inBinormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragWorldPos;
layout(location = 2) out vec3 fragWorldNormal;
layout(location = 3) out vec2 fragTexCoord;
layout(location = 4) out vec3 fragLightPos_tangent;
layout(location = 5) out vec3 fragViewPos_tangent;
layout(location = 6) out vec3 fragPos_tangent;

float outlineWidth = 0.05f;

void ExtrudeByNormal()
{
    vec3 worldPos = (pushConstants.model * vec4(inPosition, 1.0)).xyz;
    vec3 worldNormal = normalize(mat3(pushConstants.model) * inNormal);
    vec3 expandedWorldPos = worldPos + worldNormal * outlineWidth;
    gl_Position = ubo.proj * ubo.view * vec4(expandedWorldPos, 1.0);
    
    fragTexCoord = inTexCoord;
    fragWorldPos = expandedWorldPos;
    fragWorldNormal = worldNormal;
    fragColor = inColor;
}

void ExtrudeByScale()
{
    mat4 scaleMatrix = mat4(
    vec4(1.0 + outlineWidth, 0.0, 0.0, 0.0),
    vec4(0.0, 1.0 + outlineWidth, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0 + outlineWidth, 0.0),
    vec4(0.0, 0.0, 0.0, 1.0)
    );
    
    mat4 outlineMatrix = pushConstants.model * scaleMatrix;
    vec4 worldPosition = outlineMatrix * vec4(inPosition, 1.0);
    vec3 worldNormal = normalize(mat3(pushConstants.model) * inNormal);
        
    gl_Position = ubo.proj * ubo.view * worldPosition;
    fragTexCoord = inTexCoord;
    fragWorldPos = worldPosition.xyz;
    fragWorldNormal = worldNormal;
    fragColor = inColor;
}

void main() {
    //ExtrudeByNormal();
    ExtrudeByScale();
}