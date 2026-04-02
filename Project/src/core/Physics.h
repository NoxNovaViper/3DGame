#pragma once
#include <glm/glm.hpp>

class Physics {
public:
    float gravity = -9.8f;
    float velocityY = 0.0f;
    bool isGrounded = false;
    float jumpForce = 5.0f;

    // Apply gravity to a position
    void ApplyGravity(glm::vec3& position, float deltaTime);
    
    // Simple floor collision
    void CheckFloor(glm::vec3& position, float floorHeight);
    
    // Jump functionality
    void ApplyJumpForce();
    
    // Check if grounded
    bool IsGrounded() const;
};