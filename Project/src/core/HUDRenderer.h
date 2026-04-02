#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

// Immediate-mode 2D HUD renderer.
// All coordinates are in screen pixels (0,0 = top-left).
// Usage:
//   hudRenderer.Begin(screenW, screenH);
//   hudRenderer.DrawRect(...);
//   hudRenderer.DrawDiamond(...);
//   hudRenderer.DrawHealthBar(...);
//   hudRenderer.End();

class HUDRenderer {
public:
    HUDRenderer() = default;
    ~HUDRenderer();

    // Call once after GL context is ready
    void Init();

    // Call before any Draw* calls each frame
    void Begin(int screenWidth, int screenHeight);

    // Call after all Draw* calls to flush & restore state
    void End();

    // Filled axis-aligned rectangle
    void DrawRect(float x, float y, float w, float h, const glm::vec3& color, float alpha = 1.0f);

    // Hollow or filled diamond (rotated square) — SAO lock-on reticle
    void DrawDiamond(float cx, float cy, float radius, const glm::vec3& color, float alpha = 1.0f);

    // Two-tone health bar: background + filled portion
    void DrawHealthBar(float x, float y, float w, float h,
                       float fillRatio,
                       const glm::vec3& bgColor,
                       const glm::vec3& fillColor,
                       float alpha = 1.0f);

private:
    // Shader program (compiled once in Init)
    unsigned int shaderProgram = 0;
    unsigned int VAO = 0;
    unsigned int VBO = 0;

    int screenW = 1280;
    int screenH = 720;

    // Upload a unit quad scaled/translated to pixel rect and draw it
    void DrawQuad(float x, float y, float w, float h, const glm::vec4& color);

    // Internal: compile and link the simple 2D shader
    void BuildShader();
};
