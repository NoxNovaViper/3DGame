#pragma once

#include <string>
#include "entities/Entity.h"

class Skill {
public:
    Skill(const std::string& name, int amount, int cooldown);
    virtual void use(Entity* target) = 0;
protected:
    std::string name;
    int amount;
    int cooldown;
};