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
    norm = normalize(fragWorldNormal);

    //Diffuse shading
    vec3 lightDir = normalize(ubo.lightDir);
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

// Default texture method that uses per-pixel shading and applies opacity
void DefaultTextureMethod()
{
    float opacity = pushConstants.opacity;
    vec3 litColor = PerPixelShading();
    outColor = vec4(litColor, opacity);
}


void TwoBandCelShading()
{

    // Colour Banding
    vec3 lightBandColour = vec3(1.0, 1.0, 1.0);
    vec3 shadowColour = vec3(0.1, 0.1, 0.1);
    vec3 baseColour = pushConstants.ambientMat;

    vec3 normal = normalize(fragWorldNormal);
    vec3 lightDir = normalize(ubo.lightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    // Light factor for banding effect    
    float lightFactor = step(0.5, NdotL);

    // Choose between light and shadow colours
    vec3 finalColour = mix(shadowColour, lightBandColour, lightFactor);
    finalColour *= baseColour;
    
    outColor = vec4(finalColour, 1.0);
}

void ThreeBandCelShading()
{
    vec3 normal = normalize(fragWorldNormal);
    vec3 lightDir = normalize(ubo.lightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    // Light factor for banding effect    
    float lightFactor = step(0.5, NdotL);
    // Three bands: 0.0-0.33, 0.33-0.66, 0.66-1.0
    float band1 = step(0.33, NdotL);  // 1 if >= 0.33
    float band2 = step(0.66, NdotL);  // 1 if >= 0.66

    // Select colour based on bands
    vec3 colour;
    if (band2 > 0.5) {
        colour = vec3(1.0, 0.9, 0.7); // Bright
    } else if (band1 > 0.5) {
        colour = vec3(0.6, 0.5, 0.3); // Mid
    } else {
        colour = vec3(0.2, 0.15, 0.1); // Shadow
    }
    
    outColor = vec4(colour, 1.0);
}

void LerpCelShading()
{
    // Colour Banding
    vec3 shadowColour = vec3(0.1, 0.1, 0.1);
    vec3 lightBandColour = vec3(1.0, 1.0, 1.0);
    vec3 baseColour = pushConstants.ambientMat;

    vec3 normal = normalize(fragWorldNormal);
    vec3 lightDir = normalize(ubo.lightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);
    // Lerp between shadow and light colours based on NdotL
    float lerpStep = step(0.5, NdotL);
    float lerp = mix(0.0, 1.0, lerpStep); // Smooth transition

    vec3 finalColour = mix(shadowColour, lightBandColour, lerp);
    finalColour *= baseColour;
    outColor = vec4(finalColour, 1.0);
}

void FourBandCelShading()
{
    // Define 4 colours for the bands
    vec3 deepShadow = vec3(0.1, 0.05, 0.05);
    vec3 shadow = vec3(0.3, 0.2, 0.15);
    vec3 mid = vec3(0.7, 0.5, 0.3);
    vec3 highlight = vec3(1.0, 0.9, 0.7);
    
    vec3 normal = normalize(fragWorldNormal);
    vec3 lightDir = normalize(ubo.lightDir);
    float NdotL = max(dot(normal, lightDir), 0.0);

    // Calculate band index
    float band = step(0.25, NdotL) + step(0.5, NdotL) + step(0.75, NdotL);

    vec3 finalColour = mix(
        mix(mix(deepShadow, shadow, step(1.5, band)), 
            mid, step(2.5, band)), 
        highlight, step(3.5, band)
    );

    outColor = vec4(finalColour, 1.0);
}

void main() {           
    //DefaultTextureMethod();
    //TwoBandCelShading();
    //ThreeBandCelShading();    
    //LerpCelShading();
    FourBandCelShading();
}