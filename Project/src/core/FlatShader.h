#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

// Flat-color vertex shader: reads position (loc 0) and color (loc 1)
static const char* flatVertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 VertexColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    VertexColor = aColor;
}
)";

// Flat-color fragment shader: outputs interpolated vertex color
static const char* flatFragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 VertexColor;

void main() {
    FragColor = vec4(VertexColor, 1.0);
}
)";
