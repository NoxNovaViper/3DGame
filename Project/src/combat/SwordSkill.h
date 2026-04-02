#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

enum class SwordSkillType {
    LINEAR,           // Single thrust
    HORIZONTAL_SQUARE, // Horizontal slash
    VERTICAL_SQUARE,   // Vertical slash
    RAGE_SPIKE,        // Upward thrust
    SONIC_LEAP,        // Leaping strike
    STARBURST_STREAM,  // 16-hit combo (ultimate)
    ECLIPSE,           // Dual blade skill
    OBLIVION_EDGE      // Counter skill
};

struct SwordSkill {
    std::string name;
    SwordSkillType type;
    float damageMultiplier = 1.0f;
    float cooldown = 1.0f;
    float duration = 0.5f;  // How long the skill animation plays
    float range = 2.0f;     // Attack range
    int hitCount = 1;       // Number of hits in the skill
    bool isDualBlade = false;
    bool requiresUnlock = false;
    int requiredLevel = 1;

    SwordSkill();
    SwordSkill(const std::string& name, SwordSkillType type, float dmgMult, float cd, float dur, float rng, int hits);
    
    static SwordSkill CreateLinear();
    static SwordSkill CreateHorizontalSquare();
    static SwordSkill CreateVerticalSquare();
    static SwordSkill CreateRageSpike();
    static SwordSkill CreateSonicLeap();
    static SwordSkill CreateStarburstStream();
    static SwordSkill CreateEclipse();
};
