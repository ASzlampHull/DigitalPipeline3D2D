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
layout(location = 7) in float fragAge;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;



void main() {        
    /*float opacity = pushConstants.opacity;
    if (pushConstants.isVertexShaded) 
    {
        outColor = vec4(fragColor * texture(texSampler, fragTexCoord).rgb, opacity);
    }
    else
    {
        vec3 litColor = PerPixelShading();
        outColor = vec4(litColor, opacity);
    }*/

    //Colour Particles
    vec3 color;
    // Fire gradient: white -> yellow -> orange -> red -> dark red/black
    if (fragAge < 0.05)
	color = mix(vec3(1.0, 1.0, 1.0), vec3(1.0, 0.95, 0.5), fragAge / 0.15); // white to pale yellow
    else if (fragAge < 0.15)
	    color = mix(vec3(1.0, 0.95, 0.5), vec3(1.0, 0.7, 0.0), (fragAge - 0.15) / 0.2); // pale yellow to orange
    else if (fragAge < 0.25)
	    color = mix(vec3(1.0, 0.7, 0.0), vec3(1.0, 0.3, 0.0), (fragAge - 0.35) / 0.3); // orange to deep orange
    else if (fragAge > 0.35)
	    color = mix(vec3(1.0, 0.3, 0.0), vec3(0.5, 0.05, 0.0), (fragAge - 0.65) / 0.2); // deep orange to red
    else
	    color = mix(vec3(0.5, 0.05, 0.0), vec3(0.1, 0.0, 0.0), (fragAge - 0.85) / 0.15); // red to dark

    color = color * (1.0 - smoothstep(0.4, 0.5, length(fragTexCoord - 0.5)));

    // Alpha fades out as particle ages
    float alpha = 1.0 - fragAge;

    outColor = vec4(color, alpha);
}