#pragma once
#include "entities/Entity.h"
#include "combat/Hitbox.h"
#include <string>

enum class MonsterType {
    WOLF,
    BOAR,
    GOBLIN,
    ORC,
    SKELETON,
    SLIME,
    BOSS
};

enum class MonsterState {
    IDLE,
    PATROLLING,
    CHASING,
    ATTACKING,
    STUNNED,
    DEAD
};

class Monster : public Entity {
public:
    std::string name;
    MonsterType type;
    MonsterState state = MonsterState::IDLE;
    
    // Stats
    int level = 1;
    float maxHP = 50.0f;
    float currentHP = 50.0f;
    float attack = 8.0f;
    float defense = 3.0f;
    float expReward = 25.0f;
    
    // AI
    float aggroRange = 10.0f;
    float attackRange = 2.0f;
    float patrolRadius = 15.0f;
    glm::vec3 spawnPoint;
    glm::vec3 patrolTarget;
    float patrolTimer = 0.0f;
    
    // Combat
    float attackCooldown = 2.0f;
    float attackTimer = 0.0f;
    float stunDuration = 0.0f;

    // Respawn state — true while waiting to revive (prevents repeated death rewards)
    bool respawning = false;
    
    // Hitbox
    Hitbox hitbox;
    
    // Visual
    glm::vec3 color = glm::vec3(1.0f);
    
    Monster();
    Monster(glm::vec3 pos, MonsterType type);
    ~Monster();
    
    void Update(float deltaTime) override;
    void TakeDamage(float amount);
    bool IsAlive() const;
    void Respawn();
    
    // AI behavior
    void UpdateAI(float deltaTime, const glm::vec3& playerPos);
    void SetState(MonsterState newState);
    
private:
    glm::vec3 GetRandomPatrolPoint() const;
    glm::vec3 MoveTowards(const glm::vec3& target, float speed, float deltaTime) const;
};
