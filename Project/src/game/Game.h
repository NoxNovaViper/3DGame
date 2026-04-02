#pragma once
#include "game/GameState.h"
#include "entities/Player.h"
#include "entities/Monster.h"
#include "combat/CombatSystem.h"
#include "world/FloorManager.h"
#include "core/Window.h"
#include "core/Shader.h"
#include "core/InputHandler.h"
#include "core/Physics.h"
#include "core/HUDRenderer.h"
#include "quest/Quest.h"
#include <vector>
#include <memory>

class Game {
public:
    Game();
    ~Game();

    void Initialize();
    void Run();
    void Shutdown();

    GameState GetCurrentState() const;
    void SetState(GameState newState);

private:
    // Core systems
    Window* window = nullptr;
    Player* player = nullptr;
    FloorManager floorManager;
    QuestManager questManager;
    CombatSystem combat;
    Physics physics;
    HUDRenderer hudRenderer;

    // Game state
    GameState currentState = GameState::EXPLORING;

    // Monsters
    std::vector<std::unique_ptr<Monster>> monsters;

    // Lock-on targeting (SAO feel)
    bool lockOnEnabled = false;
    Monster* lockedTarget = nullptr; // non-owning pointer into `monsters`
    float lockOnMaxDistance = 25.0f;
    float lockOnMaxAngleDegrees = 25.0f; // forward cone half-angle
    float lockOnAssistResponse = 10.0f; // higher = snappier camera bias

    // Third-person camera transitions while lock-on is active.
    // We only offset the camera view (not the player position used for physics).
    float thirdPersonBlend = 0.0f; // 0 = first-person, 1 = third-person
    float thirdPersonResponse = 6.0f;
    float thirdPersonDistance = 4.0f;
    float thirdPersonHeight = 1.4f;

    struct PendingAttack {
        bool active = false;
        bool isSkill = false;
        int hitsToApply = 1;
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
        float range = 2.0f;
        float damage = 0.0f;
    };

    PendingAttack pendingAttack;

    // Timing
    float monsterSpawnTimer = 0.0f;

    // Methods
    void ProcessInput();
    void Update();
    void Render();

    void SpawnMonsters();
    void UpdateMonsters();
    void HandleCombat();
    void HandleFloorTransition();

    void OnPlayerDeath();

    void ToggleLockOn();
    void HandleLockOn(float deltaTime);
    bool IsValidLockTarget(const Monster* target) const;
    Monster* AcquireLockTarget();
    glm::vec3 GetLockAimPoint(const Monster* target) const;

    void QueuePlayerAttack(bool isSkill, const glm::vec3& direction, float range, float damage, int hitsToApply);
    bool HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const;

    std::vector<Monster*> GetValidLockCandidates() const;
    void CycleLockOnTarget(int direction);

    void DrawLockOnHUD();
};
