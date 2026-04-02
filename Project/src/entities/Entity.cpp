#include "entities/Entity.h"
#include <glm/gtc/matrix_transform.hpp>

Entity::Entity() 
    : Position(0.0f), Rotation(0.0f), Scale(1.0f) {}

Entity::Entity(glm::vec3 pos) 
    : Position(pos), Rotation(0.0f), Scale(1.0f) {}

Entity::~Entity() {}

void Entity::Update(float deltaTime) {
    // Base implementation does nothing
}

glm::mat4 Entity::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, Position);
    model = glm::rotate(model, glm::radians(Rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(Rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(Rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, Scale);
    return model;
}
