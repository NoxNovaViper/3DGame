#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Entity {
public:
    glm::vec3 Position;
    glm::vec3 Rotation;  // Euler angles (degrees)
    glm::vec3 Scale;

    Entity();
    Entity(glm::vec3 pos);
    virtual ~Entity();

    virtual void Update(float deltaTime);
    
    glm::mat4 GetModelMatrix() const;

protected:
    bool isActive = true;
};
