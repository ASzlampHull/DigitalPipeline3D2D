#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 eyePos;
    vec3 lightDir;
    vec3 lightColor;
    float time;
} ubo;

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
layout(location = 7) flat in uint fragTriangleId[];
layout(location = 8) in float fragDotProduct[]; // Input dot product for feature detection

layout(location = 0) out vec3 outColor;          // to fragment shader
layout(location = 1) out vec3 outWorldPos;
layout(location = 2) out vec3 outWorldNormal;
layout(location = 3) out vec2 outTexCoord;
layout(location = 4) out vec3 outLightPos_tangent;
layout(location = 5) out vec3 outViewPos_tangent;
layout(location = 6) out vec3 outPos_tangent;
layout(location = 7) flat out uint outTriangleId;
layout(location = 8) out float outDotProduct; // Output dot product for feature detection
// Output structure for feature segments
struct SegmentOutput {
    vec3 startPos;
    vec3 endPos;
    uint triID;
    uint edgeType; // 0=silhouette, 1=contour, 2=crease, 3=boundary
};
layout(location = 9) out SegmentOutput outSeg;

// The threshold for detecting the edges of the silhouette
float silhouetteThreshold = 0.5f;

bool IsSilhouette(float dot0, float dot1, float dot2, float threshold) {
    // Check if triangle has vertices on both sides of silhouette
    float sign0 = sign(dot0);
    float sign1 = sign(dot1);
    float sign2 = sign(dot2);
    
    // Silhouette if signs aren't all the same and threshold is near
    if (sign0 != sign1 || sign1 != sign2 || sign0 != sign2) {
        // Check threshold condition
        if (abs(dot0) < threshold || abs(dot1) < threshold || abs(dot2) < threshold) {
            return true;
        }
    }
    return false;
}

bool IsCrease(float dot0, float dot1, float dot2, float creaseAngle) {
    return false;
}

void EmitSilhouetteSegment(vec3 start, vec3 end, uint triID) {
    outSeg.startPos = start;
    outSeg.endPos = end;
    outSeg.triID = triID;
    outSeg.edgeType = 0; // Silhouette
    
    gl_Position = vec4(start, 1.0);
    EmitVertex();
    gl_Position = vec4(end, 1.0);
    EmitVertex();
    EndPrimitive();
}

void DetectFeatures()
{    
    vec3 v0 = gl_in[0].gl_Position.xyz;
    vec3 v1 = gl_in[1].gl_Position.xyz;
    vec3 v2 = gl_in[2].gl_Position.xyz;
    
    float d0 = fragDotProduct[0];
    float d1 = fragDotProduct[1];
    float d2 = fragDotProduct[2];
    
    // Detect silhouette edges
    if (IsSilhouette(d0, d1, d2, silhouetteThreshold)) {
        // Find which edge crosses silhouette
        if (sign(d0) != sign(d1)) {
            // Edge between v0 and v1 crosses silhouette
            float t = d0 / (d0 - d1);
            vec3 intersection = mix(v0, v1, t);
            EmitSilhouetteSegment(v0, v1, fragTriangleId[0]);
        }
        if (sign(d1) != sign(d2)) {
            // Edge between v1 and v2 crosses silhouette
        }
        if (sign(d2) != sign(d0)) {
            // Edge between v2 and v0 crosses silhouette
        }
    }
}

void DefaultRendering()
{
    for (int i = 0; i < 3; ++i) {
        gl_Position = gl_in[i].gl_Position;
        outWorldNormal = fragWorldNormal[i]; // Pass through attribute
        outColor = fragColor[i];
        outTriangleId = fragTriangleId[i];
        EmitVertex();
    }
    EndPrimitive();
}

void main() {
    //DefaultRendering();
    DetectFeatures();
}