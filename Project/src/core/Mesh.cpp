#include "core/Mesh.h"

// Vertex format: position (x,y,z) + color (r,g,b) — 6 floats per vertex.
// Use with flatVertexShaderSource / flatFragmentShaderSource from core/FlatShader.h.

Mesh::Mesh() {
    float vertices[] = {
        // Back face
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,

        // Front face
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,

        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.2f,
        -0.5f,  0.5f, -0.5f,  0.5f, 1.0f, 0.2f,
        -0.5f, -0.5f, -0.5f,  0.2f, 0.5f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.2f, 0.5f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.2f, 0.5f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.5f, 0.2f,

        // Right face
         0.5f,  0.5f,  0.5f,  0.3f, 0.7f, 0.3f,
         0.5f,  0.5f, -0.5f,  0.7f, 0.3f, 0.7f,
         0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f,
         0.5f, -0.5f,  0.5f,  0.2f, 0.2f, 0.8f,
         0.5f,  0.5f,  0.5f,  0.3f, 0.7f, 0.3f,

        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.4f, 0.4f, 0.4f,
         0.5f, -0.5f, -0.5f,  0.6f, 0.6f, 0.6f,
         0.5f, -0.5f,  0.5f,  0.8f, 0.8f, 0.8f,
         0.5f, -0.5f,  0.5f,  0.8f, 0.8f, 0.8f,
        -0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,  0.4f, 0.4f, 0.4f,

        // Top face
        -0.5f,  0.5f, -0.5f,  0.8f, 0.2f, 0.2f,
         0.5f,  0.5f, -0.5f,  0.2f, 0.8f, 0.2f,
         0.5f,  0.5f,  0.5f,  0.2f, 0.2f, 0.8f,
         0.5f,  0.5f,  0.5f,  0.2f, 0.2f, 0.8f,
        -0.5f,  0.5f,  0.5f,  0.8f, 0.8f, 0.2f,
        -0.5f,  0.5f, -0.5f,  0.8f, 0.2f, 0.2f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void Mesh::Draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Mesh::Cleanup() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
