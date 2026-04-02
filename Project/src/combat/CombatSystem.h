#pragma once
#include "combat/SwordSkill.h"
#include "combat/Hitbox.h"
#include "entities/Entity.h"
#include <vector>
#include <functional>

struct DamageNumber {
    glm::vec3 position;
    float value;
    float lifetime;
    float maxHeight;
    bool isCritical;
};

class CombatSystem {
public:
    CombatSystem();
    ~CombatSystem();

    void Update(float deltaTime);
    
    // Player attacks
    void PerformBasicAttack(Entity& attacker, const glm::vec3& direction, float range, float damage);
    void PerformSwordSkill(Entity& attacker, const SwordSkill& skill, const glm::vec3& direction);
    
    // Check if skill is active
    bool IsSkillActive() const;
    float GetSkillProgress() const;  // 0.0 to 1.0
    
    // Damage numbers
    void AddDamageNumber(glm::vec3 position, float damage, bool isCritical = false);
    const std::vector<DamageNumber>& GetDamageNumbers() const;
    
    // Cooldown management
    void StartCooldown(float duration);
    bool IsOnCooldown() const;
    float GetCooldownProgress() const;

private:
    bool skillActive = false;
    float skillTimer = 0.0f;
    float skillDuration = 0.0f;
    SwordSkill currentSkill;
    
    bool onCooldown = false;
    float cooldownTimer = 0.0f;
    float cooldownDuration = 0.0f;
    
    std::vector<DamageNumber> damageNumbers;
    
    void UpdateDamageNumbers(float deltaTime);
};
