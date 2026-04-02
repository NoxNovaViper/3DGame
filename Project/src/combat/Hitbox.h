#pragma once
#include <glm/glm.hpp>

struct Hitbox {
    glm::vec3 center;
    glm::vec3 size;  // Half-extents
    bool isActive = false;
    float softMargin = 0.1f; // Soft margin for more forgiving collisions

    Hitbox();
    Hitbox(glm::vec3 center, glm::vec3 size, float margin = 0.1f);

    bool Intersects(const Hitbox& other) const;
    void UpdatePosition(glm::vec3 newCenter);
};
