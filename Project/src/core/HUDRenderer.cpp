#include "core/HUDRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

// ---- minimal 2D shaders ----

static const char* kHUDVertSrc = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform mat4 uProjection;
uniform mat4 uModel;
void main() {
    gl_Position = uProjection * uModel * vec4(aPos, 0.0, 1.0);
}
)";

static const char* kHUDFragSrc = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 uColor;
void main() {
    FragColor = uColor;
}
)";

// ---- helpers ----

static unsigned int CompileShader(GLenum type, const char* src) {
    unsigned int id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    int ok; glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << "[HUDRenderer] Shader error: " << log << std::endl;
    }
    return id;
}

// ---- public ----

HUDRenderer::~HUDRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

void HUDRenderer::Init() {
    BuildShader();

    // Unit quad: two triangles covering (0,0)-(1,1)
    float verts[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void HUDRenderer::Begin(int w, int h) {
    screenW = w;
    screenH = h;
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shaderProgram);

    // Orthographic: pixel coords, Y down
    glm::mat4 proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f, -1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uProjection"), 1, GL_FALSE, glm::value_ptr(proj));
}

void HUDRenderer::End() {
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void HUDRenderer::DrawRect(float x, float y, float w, float h, const glm::vec3& color, float alpha) {
    DrawQuad(x, y, w, h, glm::vec4(color, alpha));
}

void HUDRenderer::DrawDiamond(float cx, float cy, float radius, const glm::vec3& color, float alpha) {
    // Draw four triangles forming a diamond using thin rects rotated 45°
    // Simpler: draw four thin lines as rects along the diamond edges
    float hs = radius * 0.5f; // half-size of the diamond span
    float lw = 2.0f;          // line width in pixels

    // Top-right edge
    DrawQuad(cx,        cy - hs, hs, lw, glm::vec4(color, alpha));
    // Top-left edge
    DrawQuad(cx - hs,   cy - hs, hs, lw, glm::vec4(color, alpha));
    // Bottom-right edge
    DrawQuad(cx,        cy + hs, hs, lw, glm::vec4(color, alpha));
    // Bottom-left edge
    DrawQuad(cx - hs,   cy + hs, hs, lw, glm::vec4(color, alpha));
    // Left point
    DrawQuad(cx - hs,   cy - hs, lw, hs * 2.0f, glm::vec4(color, alpha));
    // Right point
    DrawQuad(cx + hs,   cy - hs, lw, hs * 2.0f, glm::vec4(color, alpha));
}

void HUDRenderer::DrawHealthBar(float x, float y, float w, float h,
                                 float fillRatio,
                                 const glm::vec3& bgColor,
                                 const glm::vec3& fillColor,
                                 float alpha) {
    // Background
    DrawQuad(x, y, w, h, glm::vec4(bgColor, alpha));
    // Fill
    float fillW = w * std::max(0.0f, std::min(1.0f, fillRatio));
    if (fillW > 0.0f) {
        DrawQuad(x, y, fillW, h, glm::vec4(fillColor, alpha));
    }
}

// ---- private ----

void HUDRenderer::DrawQuad(float x, float y, float w, float h, const glm::vec4& color) {
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
    model = glm::scale(model, glm::vec3(w, h, 1.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform4fv(glGetUniformLocation(shaderProgram, "uColor"), 1, glm::value_ptr(color));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void HUDRenderer::BuildShader() {
    unsigned int vert = CompileShader(GL_VERTEX_SHADER,   kHUDVertSrc);
    unsigned int frag = CompileShader(GL_FRAGMENT_SHADER, kHUDFragSrc);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vert);
    glAttachShader(shaderProgram, frag);
    glLinkProgram(shaderProgram);
    int ok; glGetProgramiv(shaderProgram, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        std::cerr << "[HUDRenderer] Link error: " << log << std::endl;
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
}
