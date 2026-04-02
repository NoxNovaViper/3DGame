#include "core/Camera.h"
#include <string>
Camera::Camera(glm::vec3 startPos) {
    Position = startPos;
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f); // Define which way is actually "Up"
    Yaw = -90.0f; 
    Pitch = 0.0f;
    MovementSpeed = 2.5f; // Add a default speed
    Front = glm::vec3(0.0f, 0.0f, -1.0f);
    
    // We need to calculate the initial Right and Up vectors
    glm::vec3 right = glm::cross(Front, WorldUp);
    Right = glm::normalize(right);
    Up = glm::normalize(glm::cross(Right, Front));
}

glm::mat4 Camera::GetViewMatrix() const {
    glm::vec3 eye = Position + viewOffset;
    return glm::lookAt(eye, eye + Front, Up);
}

// THIS WAS MISSING: The Linker couldn't find this function!
void Camera::ProcessKeyboard(const char* direction, float deltaTime) {
    float velocity = MovementSpeed * deltaTime;
    if (std::string(direction) == "FORWARD")  Position += Front * velocity;
    if (std::string(direction) == "BACKWARD") Position -= Front * velocity;
    if (std::string(direction) == "LEFT")     Position -= Right * velocity;
    if (direction == std::string("RIGHT"))    Position += Right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    Yaw += xoffset * 0.1f; // Added a small sensitivity multiplier
    Pitch += yoffset * 0.1f;

    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    direction.y = sin(glm::radians(Pitch));
    direction.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(direction);
    
    // RE-CALCULATE Right and Up so WASD moves relative to where you look
    Right = glm::normalize(glm::cross(Front, WorldUp));  
    Up    = glm::normalize(glm::cross(Right, Front));
}