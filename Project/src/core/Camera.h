#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;    // Make sure Right is here too
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;

    Camera(glm::vec3 startPos);
    glm::mat4 GetViewMatrix() const;
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessKeyboard(const char* direction, float deltaTime);

    // Third-person camera offset applied on top of Position when computing the view matrix.
    // Set to (0,0,0) for first-person. Game::HandleLockOn manages this smoothly.
    void SetViewOffset(const glm::vec3& offset) { viewOffset = offset; }
    glm::vec3 GetViewOffset() const { return viewOffset; }

private:
    glm::vec3 viewOffset = glm::vec3(0.0f);
};