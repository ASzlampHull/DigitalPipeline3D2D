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

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragWorldPos;
layout(location = 2) in vec3 fragWorldNormal;
layout(location = 3) in vec2 fragTexCoord;
layout(location = 4) in vec3 fragLightPos_tangent;
layout(location = 5) in vec3 fragViewPos_tangent;
layout(location = 6) in vec3 fragPos_tangent;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;

vec3 PerPixelShading()
{
    //Amient shading
    vec3 lightColor = ubo.lightColor;
    vec3 ambientMaterial = pushConstants.ambientMat;
    vec3 norm = normalize(fragWorldNormal);

    //Diffuse shading
    vec3 lightDir = normalize(ubo.lightDir - fragWorldPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    vec3 diffMaterial = pushConstants.diffuseMat;

    // Specular shading
    vec3 viewDir = normalize(ubo.eyePos - fragWorldPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float shininess = pushConstants.shininess;
    float spec = pow(max(dot(reflectDir, viewDir), 0.0), shininess);
    vec3 specularMaterial = pushConstants.specularMat;
    
    // Emissive material
    vec3 emissiveMaterial = pushConstants.emissiveMat;

    vec3 ambientLight = ambientMaterial * lightColor;
    vec3 diffuseLight = diffMaterial * lightColor * diffuse;
    vec3 specularLight = specularMaterial * lightColor * spec;
    vec3 emissiveLight = emissiveMaterial * lightColor;
    vec3 textureLight = texture(texSampler, fragTexCoord).rgb * fragColor;
    vec3 litColor = (ambientLight + diffuseLight + specularLight + emissiveLight) * textureLight;

    return litColor;
}

void main() {        
    //float opacity = pushConstants.opacity;

    //vec3 litColor = PerPixelShading();
    //outColor = vec4(litColor, opacity);
    outColor = vec4(0.0, 0.0, 0.0, 1.0);
}