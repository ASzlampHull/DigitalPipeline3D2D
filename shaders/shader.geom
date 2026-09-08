#version 450
#extension GL_ARB_separate_shader_objects : enable

// Input: triangles from vertex shader
layout(triangles) in;
// Output: triangles to rasterizer
layout(triangle_strip, max_vertices = 3) out;

// pass through attributes
layout(location = 0) in vec3 fragColor[];        // from vertex shader
layout(location = 1) in vec3 fragWorldPos[];
layout(location = 2) in vec3 fragWorldNormal[];
layout(location = 3) in vec2 fragTexCoord[];
layout(location = 4) in vec3 fragLightPos_tangent[];
layout(location = 5) in vec3 fragViewPos_tangent[];
layout(location = 6) in vec3 fragPos_tangent[];
layout(location = 7) in uint fragTriangleId[];

layout(location = 0) out vec3 outColor;          // to fragment shader
layout(location = 1) out vec3 outWorldPos;
layout(location = 2) out vec3 outWorldNormal;
layout(location = 3) out vec2 outTexCoord;
layout(location = 4) out vec3 outLightPos_tangent;
layout(location = 5) out vec3 outViewPos_tangent;
layout(location = 6) out vec3 outPos_tangent;
layout(location = 7) out uint outTriangleId;

// The threshold for detecting the edges of the silhouette
float silhouetteThreshold;

void main() {
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;
        outWorldNormal = fragWorldNormal[i]; // Pass through attribute
        outColor = fragColor[i];
        outTriangleId = fragTriangleId[i];
        EmitVertex();
    }
    EndPrimitive();
}