#pragma once
#include "entities/Entity.h"
#include "core/Camera.h"
#include <string>
#include <vector>

struct PlayerStats {
    int level = 1;
    float maxHP = 100.0f;
    float currentHP = 100.0f;
    float attack = 10.0f;
    float defense = 5.0f;
    float speed = 5.0f;
    int col = 0;  // Crystal of Life columns unlocked
};

struct Item {
    std::string name;
    int id;
    int quantity;
    bool isStackable;
    std::string type; // e.g., "weapon", "armor", "consumable", "block"
};

struct EquipmentSlot {
    std::string type; // e.g., "helmet", "chestplate", "weapon"
    Item* item;
};

class CombatSystem;  // Forward declaration (used via pointer)
class Physics;        // Forward declaration (used via pointer)
class NPC; // Forward declaration

class Inventory {
public:
    std::vector<Item> items;
    std::vector<EquipmentSlot> equipmentSlots;
    std::vector<Item> hotbar;
    int selectedHotbarSlot;

    Inventory();
    void AddItem(const Item& item);
    void RemoveItem(int itemId, int quantity = 1);
    void EquipItem(int itemId);
    void UnequipItem(const std::string& slotType);
    void SelectHotbarSlot(int slot);
};

class Player : public Entity {
public:
    Camera camera;
    PlayerStats stats;
    std::string name = "Kirito";

    // Movement state
    bool isSprinting = false;
    bool isJumping = false;
    bool isCrouching = false;
    bool isSwimming = false;

    // Combat state
    CombatSystem* combatSystem = nullptr;
    bool isBlocking = false;
    float blockCooldown = 0.0f;
    float criticalHitChance = 0.1f;

    // Inventory
    Inventory inventory;

    // Physics
    Physics* physics = nullptr;

    Player(glm::vec3 startPos);
    ~Player();

    void Update(float deltaTime) override;
    
    void TakeDamage(float amount);
    void Heal(float amount);
    void GainEXP(float amount);
    
    bool IsAlive() const;
    void Reset(glm::vec3 spawnPoint);

    // Combat methods
    void PerformMeleeAttack(const glm::vec3& direction);
    void PerformRangedAttack(const glm::vec3& direction);
    void Block();
    void UpdateCombat(float deltaTime);

    // Movement methods
    void HandleMovement(float deltaTime, const glm::vec3& inputDirection);
    void Jump();
    void Crouch(bool enable);
    void Sprint(bool enable);
    void Swim(bool enable);
    void UpdatePhysics(float deltaTime);

    // Interaction methods
    void Interact();
    void BreakBlock();
    void PlaceBlock();
    void StartDialog(NPC* npc);
    void TradeWithMerchant(NPC* merchant, int itemId, int quantity);

private:
    float exp = 0.0f;
    float expToNextLevel = 100.0f;
};
