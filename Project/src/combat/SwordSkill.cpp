#include "combat/SwordSkill.h"

SwordSkill::SwordSkill() 
    : name("Basic Attack"), type(SwordSkillType::LINEAR), 
      damageMultiplier(1.0f), cooldown(0.5f), duration(0.3f), 
      range(2.0f), hitCount(1), isDualBlade(false), 
      requiresUnlock(false), requiredLevel(1) {}

SwordSkill::SwordSkill(const std::string& name, SwordSkillType type, float dmgMult, float cd, float dur, float rng, int hits)
    : name(name), type(type), damageMultiplier(dmgMult), cooldown(cd), 
      duration(dur), range(rng), hitCount(hits), isDualBlade(false), 
      requiresUnlock(false), requiredLevel(1) {}

SwordSkill SwordSkill::CreateLinear() {
    return SwordSkill("Linear", SwordSkillType::LINEAR, 1.5f, 1.0f, 0.3f, 2.5f, 1);
}

SwordSkill SwordSkill::CreateHorizontalSquare() {
    return SwordSkill("Horizontal Square", SwordSkillType::HORIZONTAL_SQUARE, 1.8f, 1.5f, 0.5f, 3.0f, 2);
}

SwordSkill SwordSkill::CreateVerticalSquare() {
    return SwordSkill("Vertical Square", SwordSkillType::VERTICAL_SQUARE, 2.0f, 1.5f, 0.5f, 3.0f, 2);
}

SwordSkill SwordSkill::CreateRageSpike() {
    return SwordSkill("Rage Spike", SwordSkillType::RAGE_SPIKE, 2.5f, 2.0f, 0.6f, 3.5f, 1);
}

SwordSkill SwordSkill::CreateSonicLeap() {
    return SwordSkill("Sonic Leap", SwordSkillType::SONIC_LEAP, 2.2f, 2.5f, 0.7f, 5.0f, 1);
}

SwordSkill SwordSkill::CreateStarburstStream() {
    SwordSkill skill("Starburst Stream", SwordSkillType::STARBURST_STREAM, 5.0f, 10.0f, 2.0f, 4.0f, 16);
    skill.isDualBlade = true;
    skill.requiresUnlock = true;
    skill.requiredLevel = 50;
    return skill;
}

SwordSkill SwordSkill::CreateEclipse() {
    SwordSkill skill("Eclipse", SwordSkillType::ECLIPSE, 8.0f, 15.0f, 3.0f, 5.0f, 27);
    skill.isDualBlade = true;
    skill.requiresUnlock = true;
    skill.requiredLevel = 80;
    return skill;
}
