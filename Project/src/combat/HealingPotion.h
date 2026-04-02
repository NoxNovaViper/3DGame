#pragma once
#include "combat/Skill.h"
#include "entities/Entity.h"

class HealingPotion : public Skill {
public:
    HealingPotion();
    void use(Entity* target) override;
};
