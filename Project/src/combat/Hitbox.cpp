#include "combat/Hitbox.h"
#include <algorithm>

Hitbox::Hitbox() 
    : center(0.0f), size(0.5f), softMargin(0.1f) {}

Hitbox::Hitbox(glm::vec3 center, glm::vec3 size, float margin) 
    : center(center), size(size), softMargin(margin) {}

bool Hitbox::Intersects(const Hitbox& other) const {
    if (!isActive || !other.isActive) return false;
    
    // Add soft margin to both hitboxes for more forgiving collisions
    float totalMarginX = size.x + other.size.x + softMargin + other.softMargin;
    float totalMarginY = size.y + other.size.y + softMargin + other.softMargin;
    float totalMarginZ = size.z + other.size.z + softMargin + other.softMargin;
    
    return (std::abs(center.x - other.center.x) < totalMarginX) &&
           (std::abs(center.y - other.center.y) < totalMarginY) &&
           (std::abs(center.z - other.center.z) < totalMarginZ);
}

void Hitbox::UpdatePosition(glm::vec3 newCenter) {
    center = newCenter;
}
