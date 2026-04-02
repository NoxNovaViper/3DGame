#include "core/Physics.h"

void Physics::ApplyGravity(glm::vec3& position, float deltaTime) {
    if (!isGrounded) {
        velocityY += gravity * deltaTime; // Acceleration
        position.y += velocityY * deltaTime; // Movement
    } else {
        velocityY = 0.0f;
    }
}

void Physics::CheckFloor(glm::vec3& position, float floorHeight) {
    if (position.y <= floorHeight) {
        position.y = floorHeight;
        isGrounded = true;
    } else {
        isGrounded = false;
    }
}

void Physics::ApplyJumpForce() {
    if (isGrounded) {
        velocityY = jumpForce;
        isGrounded = false;
    }
}

bool Physics::IsGrounded() const {
    return isGrounded;
}