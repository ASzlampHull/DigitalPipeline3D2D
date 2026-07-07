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

// Returns a perturbed normal using the diffuse map as a height map
vec3 BumpNormalFromDiffuse(sampler2D tex, vec2 uv, vec3 origNormal, float scale)
{
    // Compute height from luminance of the diffuse texture
    float heightC = dot(texture(tex, uv).rgb, vec3(0.299, 0.587, 0.114));
    float heightL = dot(texture(tex, uv + vec2(-0.001, 0.0)).rgb, vec3(0.299, 0.587, 0.114));
    float heightR = dot(texture(tex, uv + vec2( 0.001, 0.0)).rgb, vec3(0.299, 0.587, 0.114));
    float heightU = dot(texture(tex, uv + vec2(0.0,  0.001)).rgb, vec3(0.299, 0.587, 0.114));
    float heightD = dot(texture(tex, uv + vec2(0.0, -0.001)).rgb, vec3(0.299, 0.587, 0.114));

    // Estimate gradient
    float dx = (heightR - heightL) * scale;
    float dy = (heightU - heightD) * scale;

    // Construct tangent space normal
    vec3 n = normalize(vec3(-dx, -dy, 1.0));
    // Optionally transform to world space if you have TBN, otherwise just perturb the original normal
    // Here, we simply blend with the original normal for a cheap effect
    return normalize(mix(origNormal, n, 0.5));
}

vec3 PerPixelShading()
{
    //Amient shading
    vec3 lightColor = ubo.lightColor;
    vec3 ambientMaterial = pushConstants.ambientMat;

    //Bump mapping to perturb normals
    vec3 norm = vec3(0.0);
    if (pushConstants.isBumpHeight)
        norm = BumpNormalFromDiffuse(texSampler, fragTexCoord, fragWorldNormal, 8.0);
    else
        norm = normalize(fragWorldNormal);

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
    float opacity = pushConstants.opacity;
    if (pushConstants.isVertexShaded) 
    {
        outColor = vec4(fragColor * texture(texSampler, fragTexCoord).rgb, opacity);
    }
    else
    {
        if (pushConstants.isBumpHeight)
        {            
            float height = dot(texture(texSampler, fragTexCoord).rgb, vec3(0.5, 0.5, 0.5));
            vec3 litColor = PerPixelShading();
            outColor = vec4(litColor * height, opacity);
        }
        else
        {
            vec3 litColor = PerPixelShading();
            outColor = vec4(litColor, opacity);
        }
    }
}