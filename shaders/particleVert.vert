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
layout(location = 7) out float fragAge;

#define particleSpeed 0.48
#define particleSpread 2.48
#define particleShape 0.37
#define particleSize 0.8
#define particleSystemHeight -10.0

void main() {

    // Animate particle positions
    float t = fract(inPosition.z + particleSpeed * ubo.time); // 0..1, particle's life progress

    // New position calculation
    vec3 pos;
    pos.x = particleSpread * t * cos(50.0 * inPosition.z);
    pos.y = t * particleSystemHeight;
    pos.z = particleSpread * t * sin(120.0 * inPosition.z);

    // Optionally, scale particle size as it rises (e.g., shrink or grow)
    float size = mix(particleSize, particleSize * 0.5, t); // Shrink over time

    // Billboarding and position as before
    mat4 viewInv = inverse(ubo.view);
    vec3 BBPos = (inPosition.x * viewInv[0] + inPosition.y * viewInv[1]).xyz;
    pos += size * BBPos;

    gl_Position = ubo.proj * ubo.view * vec4(pos, 1.0);

    // Pass t to fragment shader for color/alpha
    fragTexCoord = inPosition.xy;
    fragAge = t;
        
    // Calculate Tangent Space Matrix
    mat4 ModelMatrix_TInv = transpose(inverse(ubo.model));
    vec3 T = normalize(mat3(ModelMatrix_TInv) * inTangent);
    vec3 B = normalize(mat3(ModelMatrix_TInv) * inBinormal);
    vec3 N = normalize(mat3(ModelMatrix_TInv) * inNormal);
    mat3 TBN = transpose(mat3(T, B, N));

    // Get world-space light and view positions
    vec3 lightPos_world = ubo.lightDir;
    vec3 viewPos_world = ubo.eyePos;
    vec3 fragPos_world = (ubo.model * vec4(inPosition, 1.0)).xyz;

    // Transform light and view POSITIONS to tangent space
    fragLightPos_tangent = TBN * lightPos_world;
    fragViewPos_tangent = TBN * viewPos_world;
    fragPos_tangent = TBN * fragPos_world;
}