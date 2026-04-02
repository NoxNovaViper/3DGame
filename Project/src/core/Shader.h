#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

// Vertex Shader: reads position (loc 0), normal (loc 1), and texture coordinates (loc 2)
static const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
}
)";

// Fragment Shader: outputs solid black blocks with simple lighting
static const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main() {
    // Simple directional lighting from above
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3)); // Light from above and slightly angled
    float diff = max(dot(norm, lightDir), 0.0);
    
    // Dark gray with subtle shading
    float brightness = 0.1 + (diff * 0.15); // 0.1 ambient + up to 0.25 from light
    FragColor = vec4(brightness, brightness, brightness, 1.0);
}
)";

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexSource, const char* fragmentSource);
    void use();
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setInt(const std::string &name, int value) const;
    void setBiomeType(const std::string &name, int biomeType) const;
};
