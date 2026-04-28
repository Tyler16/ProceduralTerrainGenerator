#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main() {
    TexCoords = aPos;
    // Strip translation: Sky stays centered on camera
    mat4 staticView = mat4(mat3(view)); 
    vec4 pos = projection * staticView * vec4(aPos, 1.0);
    gl_Position = pos.xyww; // Force depth to 1.0 (furthest back)
}
