#include "entities/Monster.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <iostream>

static std::default_random_engine engine(42);

Monster::Monster() 
    : Entity(), type(MonsterType::WOLF), spawnPoint(0.0f) 
{
    hitbox.size = glm::vec3(0.5f, 1.0f, 0.5f);
}

Monster::Monster(glm::vec3 pos, MonsterType type) 
    : Entity(pos), type(type), spawnPoint(pos) 
{
    hitbox.center = pos;
    hitbox.isActive = true;
    
    switch (type) {
        case MonsterType::WOLF:
            name = "Gray Wolf";
            color = glm::vec3(0.5f, 0.5f, 0.55f);
            maxHP = 50.0f;
            attack = 8.0f;
            expReward = 25.0f;
            aggroRange = 10.0f;
            break;
        case MonsterType::BOAR:
            name = "Frenzy Boar";
            color = glm::vec3(0.6f, 0.4f, 0.2f);
            maxHP = 65.0f;
            attack = 10.0f;
            expReward = 30.0f;
            aggroRange = 8.0f;
            break;
        case MonsterType::SLIME:
            name = "Slime";
            color = glm::vec3(0.2f, 0.8f, 0.3f);
            maxHP = 25.0f;
            attack = 4.0f;
            expReward = 10.0f;
            aggroRange = 6.0f;
            break;
        case MonsterType::GOBLIN:
            name = "Goblin";
            color = glm::vec3(0.3f, 0.7f, 0.2f);
            maxHP = 40.0f;
            attack = 6.0f;
            expReward = 20.0f;
            aggroRange = 8.0f;
            break;
        case MonsterType::ORC:
            name = "Orc Warrior";
            color = glm::vec3(0.4f, 0.6f, 0.3f);
            maxHP = 100.0f;
            attack = 15.0f;
            defense = 8.0f;
            expReward = 50.0f;
            aggroRange = 12.0f;
            break;
        case MonsterType::SKELETON:
            name = "Skeleton";
            color = glm::vec3(0.9f, 0.9f, 0.8f);
            maxHP = 60.0f;
            attack = 10.0f;
            expReward = 30.0f;
            aggroRange = 15.0f;
            break;
        case MonsterType::BOSS:
            name = "Floor Boss";
            color = glm::vec3(0.8f, 0.1f, 0.1f);
            maxHP = 500.0f;
            attack = 30.0f;
            defense = 15.0f;
            expReward = 500.0f;
            aggroRange = 20.0f;
            Scale = glm::vec3(2.0f);
            hitbox.size = glm::vec3(1.5f, 3.0f, 1.5f);
            break;
        default:
            name = "Monster";
            color = glm::vec3(1.0f);
            break;
    }
    
    currentHP = maxHP;
    patrolTarget = GetRandomPatrolPoint();
}

Monster::~Monster() {}

void Monster::Update(float deltaTime) {
    if (state == MonsterState::DEAD) return;
    
    if (state == MonsterState::STUNNED) {
        stunDuration -= deltaTime;
        if (stunDuration <= 0.0f) {
            SetState(MonsterState::IDLE);
        }
        return;
    }
    
    // Update attack cooldown
    if (attackTimer > 0.0f) {
        attackTimer -= deltaTime;
    }
    
    // Update hitbox position
    hitbox.center = Position;
}

void Monster::TakeDamage(float amount) {
    currentHP -= amount;
    std::cout << name << " took " << amount << " damage! HP: " 
              << currentHP << "/" << maxHP << std::endl;
    
    if (currentHP <= 0) {
        currentHP = 0;
        SetState(MonsterState::DEAD);
        std::cout << name << " has been defeated!" << std::endl;
    } else {
        // Enter chase state when damaged
        SetState(MonsterState::CHASING);
    }
}

bool Monster::IsAlive() const {
    return state != MonsterState::DEAD;
}

void Monster::Respawn() {
    respawning = true;
    Position = spawnPoint;
    currentHP = maxHP;
    SetState(MonsterState::IDLE);
    patrolTarget = GetRandomPatrolPoint();
    respawning = false; // revival complete
}

void Monster::UpdateAI(float deltaTime, const glm::vec3& playerPos) {
    if (state == MonsterState::DEAD || state == MonsterState::STUNNED) return;
    
    float distToPlayer = glm::distance(Position, playerPos);
    
    switch (state) {
        case MonsterState::IDLE:
            // Check if player is in aggro range
            if (distToPlayer < aggroRange) {
                SetState(MonsterState::CHASING);
            }
            break;
            
        case MonsterState::PATROLLING:
            // Move towards patrol target
            Position = MoveTowards(patrolTarget, 2.0f, deltaTime);
            
            // Check if reached target
            if (glm::distance(Position, patrolTarget) < 1.0f) {
                patrolTimer = 3.0f + (float)(rand() % 5);  // Wait 3-8 seconds
                SetState(MonsterState::IDLE);
            }
            
            // Check if player is in aggro range
            if (distToPlayer < aggroRange) {
                SetState(MonsterState::CHASING);
            }
            break;
            
        case MonsterState::CHASING:
            // Move towards player
            Position = MoveTowards(playerPos, 4.0f, deltaTime);
            
            // Check if in attack range
            if (distToPlayer < attackRange && attackTimer <= 0.0f) {
                SetState(MonsterState::ATTACKING);
            }
            
            // Check if player is out of range
            if (distToPlayer > aggroRange * 1.5f) {
                SetState(MonsterState::PATROLLING);
                patrolTarget = GetRandomPatrolPoint();
            }
            break;
            
        case MonsterState::ATTACKING:
            // Attack animation would play here
            attackTimer = attackCooldown;
            SetState(MonsterState::CHASING);
            break;
    }
    
    // Update hitbox
    hitbox.center = Position;
}

void Monster::SetState(MonsterState newState) {
    state = newState;
    
    switch (newState) {
        case MonsterState::IDLE:
            break;
        case MonsterState::PATROLLING:
            patrolTarget = GetRandomPatrolPoint();
            break;
        case MonsterState::CHASING:
            break;
        case MonsterState::ATTACKING:
            break;
        case MonsterState::STUNNED:
            stunDuration = 1.0f;
            break;
        case MonsterState::DEAD:
            hitbox.isActive = false;
            break;
    }
}

glm::vec3 Monster::GetRandomPatrolPoint() const {
    float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
    float radius = (float)(rand() % 100) / 100.0f * patrolRadius;
    return spawnPoint + glm::vec3(cos(angle) * radius, 0.0f, sin(angle) * radius);
}

glm::vec3 Monster::MoveTowards(const glm::vec3& target, float speed, float deltaTime) const {
    glm::vec3 direction = glm::normalize(target - Position);
    return Position + direction * speed * deltaTime;
}
