#include "entities/Player.h"
#include "entities/NPC.h"
#include "combat/CombatSystem.h"
#include "core/Physics.h"
#include <algorithm>
#include <iostream>

// Inventory methods
Inventory::Inventory() : selectedHotbarSlot(0) {
    // Initialize equipment slots
    equipmentSlots = {
        {"helmet", nullptr},
        {"chestplate", nullptr},
        {"weapon", nullptr}
    };
    hotbar.resize(9); // 9 slots for hotbar
}

void Inventory::AddItem(const Item& item) {
    // Check if item is stackable and already exists in inventory
    if (item.isStackable) {
        for (auto& existingItem : items) {
            if (existingItem.id == item.id) {
                existingItem.quantity += item.quantity;
                return;
            }
        }
    }
    items.push_back(item);
}

void Inventory::RemoveItem(int itemId, int quantity) {
    for (auto it = items.begin(); it != items.end(); ++it) {
        if (it->id == itemId) {
            it->quantity -= quantity;
            if (it->quantity <= 0) {
                items.erase(it);
            }
            return;
        }
    }
}

void Inventory::EquipItem(int itemId) {
    for (auto& item : items) {
        if (item.id == itemId) {
            // Find appropriate equipment slot
            for (auto& slot : equipmentSlots) {
                if (slot.type == item.type) {
                    slot.item = &item;
                    break;
                }
            }
            return;
        }
    }
}

void Inventory::UnequipItem(const std::string& slotType) {
    for (auto& slot : equipmentSlots) {
        if (slot.type == slotType) {
            slot.item = nullptr;
            return;
        }
    }
}

void Inventory::SelectHotbarSlot(int slot) {
    if (slot >= 0 && slot < hotbar.size()) {
        selectedHotbarSlot = slot;
    }
}

// Player methods
Player::Player(glm::vec3 startPos)
    : Entity(startPos), camera(startPos)
{
    stats.currentHP = stats.maxHP;
}

Player::~Player() {
    // CombatSystem and Physics are owned by Game (to avoid duplicating systems).
}

void Player::Update(float deltaTime) {
    Entity::Update(deltaTime);
    // Sync entity position with camera
    Position = camera.Position;
    UpdateCombat(deltaTime);
}

void Player::TakeDamage(float amount) {
    // Calculate actual damage after defense reduction
    float actualDamage = std::max(amount - stats.defense, 1.0f);  // Minimum 1 damage
    
    stats.currentHP -= actualDamage;
    std::cout << "Player took " << actualDamage << " damage! HP: "
              << stats.currentHP << "/" << stats.maxHP << std::endl;
    
    if (stats.currentHP <= 0) {
        stats.currentHP = 0;
        std::cout << "Player has died!" << std::endl;
    }
}

void Player::Heal(float amount) {
    // Prevent overhealing beyond max HP
    float healingAmount = amount;
    if (stats.currentHP + healingAmount > stats.maxHP) {
        healingAmount = stats.maxHP - stats.currentHP;
    }
    stats.currentHP += healingAmount;
    
    std::cout << "Player healed for " << healingAmount << " HP! Current HP: "
              << stats.currentHP << "/" << stats.maxHP << std::endl;
}

void Player::GainEXP(float amount) {
    exp += amount;
    std::cout << "Gained " << amount << " EXP! Total: " << exp << "/" << expToNextLevel << std::endl;
    
    while (exp >= expToNextLevel) {
        exp -= expToNextLevel;
        stats.level++;
        expToNextLevel *= 1.5f;
        
        // Stat increases on level up
        stats.maxHP += 20.0f;
        stats.currentHP = stats.maxHP;  // Full heal on level up
        stats.attack += 3.0f;
        stats.defense += 2.0f;
        stats.speed += 0.5f;
        
        std::cout << "LEVEL UP! Now level " << stats.level << std::endl;
    }
}

bool Player::IsAlive() const {
    return stats.currentHP > 0;
}

void Player::Reset(glm::vec3 spawnPoint) {
    camera.Position = spawnPoint;
    Position = spawnPoint;
    stats.currentHP = stats.maxHP;
    stats.level = 1;
    exp = 0.0f;
    expToNextLevel = 100.0f;
    isSprinting = false;
    isJumping = false;
    isCrouching = false;
    isSwimming = false;
    isBlocking = false;
}

// Combat methods
void Player::PerformMeleeAttack(const glm::vec3& direction) {
    if (combatSystem && !combatSystem->IsOnCooldown()) {
        combatSystem->PerformBasicAttack(*this, direction, 2.0f, stats.attack);
        combatSystem->StartCooldown(0.5f);
    }
}

void Player::PerformRangedAttack(const glm::vec3& direction) {
    if (combatSystem && !combatSystem->IsOnCooldown()) {
        // Placeholder for ranged attack implementation
        std::cout << "Performing ranged attack in direction: " << direction.x << ", " << direction.y << ", " << direction.z << std::endl;
        combatSystem->StartCooldown(1.0f);
    }
}

void Player::Block() {
    if (!isBlocking && blockCooldown <= 0.0f) {
        isBlocking = true;
        blockCooldown = 0.3f;
        std::cout << "Player is blocking!" << std::endl;
    }
}

void Player::UpdateCombat(float deltaTime) {
    if (isBlocking) {
        blockCooldown -= deltaTime;
        if (blockCooldown <= 0.0f) {
            isBlocking = false;
        }
    }
}

// Movement methods
void Player::HandleMovement(float deltaTime, const glm::vec3& inputDirection) {
    float currentSpeed = stats.speed;
    if (isSprinting) {
        currentSpeed *= 2.0f;
    } else if (isCrouching) {
        currentSpeed *= 0.5f;
    } else if (isSwimming) {
        currentSpeed *= 0.7f;
    }
    
    // Apply movement
    Position += inputDirection * currentSpeed * deltaTime;
    camera.Position = Position;
}

void Player::Jump() {
    if (!isJumping && physics) {
        physics->ApplyJumpForce();
        isJumping = true;
    }
}

void Player::Crouch(bool enable) {
    isCrouching = enable;
    // Adjust camera height or player hitbox here if needed
}

void Player::Sprint(bool enable) {
    isSprinting = enable;
}

void Player::Swim(bool enable) {
    isSwimming = enable;
}

void Player::UpdatePhysics(float deltaTime) {
    if (physics) {
        physics->ApplyGravity(Position, deltaTime);
        physics->CheckFloor(Position, 0.0f);
        
        if (physics->IsGrounded()) {
            isJumping = false;
        }
    }
}

// Interaction methods
void Player::Interact() {
    std::cout << "Player interacted with the environment!" << std::endl;
}

void Player::BreakBlock() {
    std::cout << "Player broke a block!" << std::endl;
}

void Player::PlaceBlock() {
    std::cout << "Player placed a block!" << std::endl;
}

void Player::StartDialog(NPC* npc) {
    if (npc) {
        std::cout << "Player started dialog with NPC: " << npc->name << std::endl;
    }
}

void Player::TradeWithMerchant(NPC* merchant, int itemId, int quantity) {
    if (merchant) {
        std::cout << "Player traded " << quantity << " of item " << itemId << " with merchant: " << merchant->name << std::endl;
    }
}
