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
layout(location = 6) in uint inTriangleId;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragWorldPos;
layout(location = 2) out vec3 fragWorldNormal;
layout(location = 3) out vec2 fragTexCoord;
layout(location = 4) out vec3 fragLightPos_tangent;
layout(location = 5) out vec3 fragViewPos_tangent;
layout(location = 6) out vec3 fragPos_tangent;
layout(location = 7) out uint fragTriangleId;

vec3 VertexShading()
{
    vec3 worldPos = (pushConstants.model * vec4(inPosition, 1.0)).xyz;
    vec3 worldNormal = mat3(transpose(inverse(pushConstants.model))) * inNormal;

    // Ambient shading
    vec3 lightColor = ubo.lightColor.xyz;
    vec3 ambientMaterial = pushConstants.ambientMat;

    // Diffuse shading
    vec3 norm = normalize(worldNormal);
    vec3 lightDir = normalize(ubo.lightDir - worldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 diffMaterial = pushConstants.diffuseMat;

    // Specular shading
    vec3 viewDir = normalize(ubo.eyePos - worldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float shininess = pushConstants.shininess;
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);
    vec3 specularMaterial = pushConstants.specularMat;

    vec3 color = ambientMaterial * lightColor;
    color += diffMaterial * lightColor * diffuse;
    color += specularMaterial * lightColor * spec;
    color *= inColor;
    return color;
}

void main() {
    gl_Position = ubo.proj * ubo.view * pushConstants.model * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
    fragWorldPos = (ubo.model * vec4(inPosition, 1.0)).xyz;
    fragWorldNormal = normalize(mat3(transpose(inverse(ubo.model))) * inNormal);
    fragTriangleId = inTriangleId;
    
    if (pushConstants.isVertexShaded)
    {
        fragColor = VertexShading();
    }
    else
    {        
        fragColor = inColor;
    }
}