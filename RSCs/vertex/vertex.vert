#version 450

layout(location = 0) in vec3 inPosition; // Vertex position input
layout(location = 1) in vec3 inColor;    // Vertex color input

layout(location = 0) out vec3 fragColor; // Output color to fragment shader

void main() {
    // Set the vertex position (pass through inPosition)
    gl_Position = vec4(inPosition, 1.0);

    // Pass the vertex color to the fragment shader
    fragColor = inColor;
}