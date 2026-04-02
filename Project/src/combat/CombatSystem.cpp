#include "combat/CombatSystem.h"
#include <iostream>
#include <algorithm>

CombatSystem::CombatSystem() {}

CombatSystem::~CombatSystem() {}

void CombatSystem::Update(float deltaTime) {
    // Update skill timer
    if (skillActive) {
        skillTimer -= deltaTime;
        if (skillTimer <= 0.0f) {
            skillActive = false;
            skillTimer = 0.0f;
        }
    }
    
    // Update cooldown
    if (onCooldown) {
        cooldownTimer -= deltaTime;
        if (cooldownTimer <= 0.0f) {
            onCooldown = false;
            cooldownTimer = 0.0f;
        }
    }
    
    // Update damage numbers
    UpdateDamageNumbers(deltaTime);
}

void CombatSystem::PerformBasicAttack(Entity& attacker, const glm::vec3& direction, float range, float damage) {
    if (onCooldown || skillActive) return;
    
    std::cout << "Basic attack! Damage: " << damage << std::endl;
    StartCooldown(0.5f);
}

void CombatSystem::PerformSwordSkill(Entity& attacker, const SwordSkill& skill, const glm::vec3& direction) {
    if (onCooldown || skillActive) return;
    
    currentSkill = skill;
    skillActive = true;
    skillTimer = skill.duration;
    skillDuration = skill.duration;
    
    std::cout << "Sword Skill: " << skill.name << "! (" << skill.hitCount << " hits)" << std::endl;
    StartCooldown(skill.cooldown);
}

bool CombatSystem::IsSkillActive() const {
    return skillActive;
}

float CombatSystem::GetSkillProgress() const {
    if (!skillActive || skillDuration == 0.0f) return 0.0f;
    return 1.0f - (skillTimer / skillDuration);
}

void CombatSystem::AddDamageNumber(glm::vec3 position, float damage, bool isCritical) {
    DamageNumber dn;
    dn.position = position;
    dn.value = damage;
    dn.lifetime = 1.5f;
    dn.maxHeight = 0.0f;
    dn.isCritical = isCritical;
    damageNumbers.push_back(dn);
}

const std::vector<DamageNumber>& CombatSystem::GetDamageNumbers() const {
    return damageNumbers;
}

void CombatSystem::StartCooldown(float duration) {
    onCooldown = true;
    cooldownTimer = duration;
    cooldownDuration = duration;
}

bool CombatSystem::IsOnCooldown() const {
    return onCooldown;
}

float CombatSystem::GetCooldownProgress() const {
    if (!onCooldown || cooldownDuration == 0.0f) return 0.0f;
    return 1.0f - (cooldownTimer / cooldownDuration);
}

void CombatSystem::UpdateDamageNumbers(float deltaTime) {
    for (auto& dn : damageNumbers) {
        dn.lifetime -= deltaTime;
        dn.maxHeight += deltaTime * 2.0f;  // Float upward
    }
    
    // Remove expired damage numbers
    damageNumbers.erase(
        std::remove_if(damageNumbers.begin(), damageNumbers.end(),
            [](const DamageNumber& dn) { return dn.lifetime <= 0.0f; }),
        damageNumbers.end()
    );
}
