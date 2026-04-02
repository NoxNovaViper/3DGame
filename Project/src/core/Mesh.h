#pragma once
#include <glad/glad.h>
#include <vector>

// Mesh uses per-vertex position (loc 0) + color (loc 1).
// Always render with a Shader built from flatVertexShaderSource / flatFragmentShaderSource
// defined in core/FlatShader.h — NOT the default biome shader in Shader.h.

class Mesh {
public:
    unsigned int VAO, VBO;

    Mesh();       // Constructor uploads cube geometry to GPU
    void Draw();
    void Cleanup();
};
