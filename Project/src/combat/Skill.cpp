#include "combat/Skill.h"

Skill::Skill(const std::string& name, int amount, int cooldown)
    : name(name), amount(amount), cooldown(cooldown) {}
