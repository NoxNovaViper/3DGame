#include "game/Game.h"
#include "core/ResourceManager.h"
#include "world/ChunkManager.h"
#include "core/Shader.h"
#include "core/FlatShader.h"
#include "world/Terrain.h"
#include "world/WorldRenderer.h"
#include "graphics/Shader.h"
#include "world/Chunk.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdlib>

Game::Game() {}

Game::~Game() {}

void Game::Initialize() {
    // 1. Create window
    window = new Window(1280, 720, "SAO: Aincrad");
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // 2. Load shaders
    ResourceManager::LoadShader("block", vertexShaderSource, fragmentShaderSource);
    
    // 3. Initialize floor manager
    floorManager.Initialize(100);
    
    // 4. Create player at spawn point
    glm::vec3 spawn = floorManager.GetFloorSpawnPoint(1);
    player = new Player(spawn);

    // Tie player actions to the single authoritative systems owned by Game.
    player->combatSystem = &combat;
    player->physics = &physics;
    
    // 5. Initialize input
    InputHandler::Init(window->handle, &player->camera);
    
    // 6. Initialize HUD renderer
    hudRenderer.Init();
    
    // 7. Spawn initial monsters
    SpawnMonsters();
    
    // 7. Initialize quest
    Quest starterQuest(1, "The Journey Begins", "Defeat 3 wolves on the first floor", QuestType::KILL);
    starterQuest.objectives[0].targetType = "wolf";
    starterQuest.objectives[0].requiredProgress = 3;
    starterQuest.expReward = 150;
    starterQuest.colReward = 50;
    starterQuest.Start();
    questManager.AddQuest(starterQuest);
    
    std::cout << "SAO: Aincrad initialized!" << std::endl;
    std::cout << "Floor 1: " << floorManager.GetCurrentFloorPtr()->config.name << std::endl;
}

void Game::Run() {
    while (!window->shouldClose()) {
        // A. Time
        window->updateTime();
        
        // B. Process input
        ProcessInput();
        
        // C. Update game state
        Update();
        
        // D. Render
        Render();
        
        // E. Swap buffers
        window->swapBuffers();
    }
}

void Game::Shutdown() {
    ResourceManager::Clear();
    delete player;
    delete window;
}

GameState Game::GetCurrentState() const {
    return currentState;
}

void Game::SetState(GameState newState) {
    currentState = newState;
}

void Game::ProcessInput() {
    if (!player) return;
    
    // Store position before movement for wall collision
    glm::vec3 prevPos = player->camera.Position;
    
    InputHandler::ProcessInput(window->handle, window->deltaTime);
    
    // Wall collision - only check X/Z, not Y (floor handled separately)
    Floor* currentFloor = floorManager.GetCurrentFloorPtr();
    if (currentFloor) {
        // Check at feet level (where the player actually is)
        int bx = (int)std::floor(player->camera.Position.x);
        int by = (int)std::floor(player->camera.Position.y - 1.5f); // Player feet
        int bz = (int)std::floor(player->camera.Position.z);
        
        // Check horizontal collision (walls only, not floor)
        if (currentFloor->GetBlock(bx, by, bz) != BLOCK_AIR) {
            // Revert X and Z, keep Y for jumping/falling
            player->camera.Position.x = prevPos.x;
            player->camera.Position.z = prevPos.z;
        }
        // Also check at head level
        int byHead = (int)std::floor(player->camera.Position.y - 0.5f);
        if (currentFloor->GetBlock(bx, byHead, bz) != BLOCK_AIR) {
            player->camera.Position.x = prevPos.x;
            player->camera.Position.z = prevPos.z;
        }
    }
    
    // Lock-on toggle - SAO-style aim assist
    if (InputHandler::OnKeyPress(GLFW_KEY_TAB)) {
        ToggleLockOn();
    }

    // Lock-on target cycling (only meaningful when already locked)
    if (lockOnEnabled) {
        if (InputHandler::OnKeyPress(GLFW_KEY_V)) {
            CycleLockOnTarget(+1);
        }
        if (InputHandler::OnKeyPress(GLFW_KEY_B)) {
            CycleLockOnTarget(-1);
        }
    }

    // Combat input - left click for basic attack
    if (InputHandler::OnKeyPress(GLFW_MOUSE_BUTTON_1)) {
        if (currentState == GameState::EXPLORING || currentState == GameState::COMBAT) {
            // Get camera forward direction
            glm::vec3 forward = player->camera.Front;
            if (lockOnEnabled && lockedTarget && lockedTarget->IsAlive()) {
                glm::vec3 toTarget = GetLockAimPoint(lockedTarget) - player->camera.Position;
                if (glm::length(toTarget) > 0.0001f) {
                    forward = glm::normalize(toTarget);
                }
            }
            QueuePlayerAttack(false, forward, 2.5f, player->stats.attack, 1);
            combat.PerformBasicAttack(*player, forward, 2.5f, player->stats.attack);
            SetState(GameState::COMBAT);
        }
    }
    
    // Sword skill - right click
    if (InputHandler::OnKeyPress(GLFW_MOUSE_BUTTON_2)) {
        if (currentState == GameState::EXPLORING || currentState == GameState::COMBAT) {
            SwordSkill skill = SwordSkill::CreateLinear();
            glm::vec3 forward = player->camera.Front;
            if (lockOnEnabled && lockedTarget && lockedTarget->IsAlive()) {
                glm::vec3 toTarget = GetLockAimPoint(lockedTarget) - player->camera.Position;
                if (glm::length(toTarget) > 0.0001f) {
                    forward = glm::normalize(toTarget);
                }
            }
            // Scale skill damage by the configured multiplier.
            float skillDamage = player->stats.attack * skill.damageMultiplier;
            QueuePlayerAttack(true, forward, skill.range, skillDamage, skill.hitCount);
            combat.PerformSwordSkill(*player, skill, forward);
            SetState(GameState::COMBAT);
        }
    }
    
    // Floor transition - press F
    if (InputHandler::OnKeyPress(GLFW_KEY_F)) {
        HandleFloorTransition();
    }
    
    // Pause - press Escape
    if (InputHandler::OnKeyPress(GLFW_KEY_ESCAPE)) {
        if (currentState == GameState::EXPLORING || currentState == GameState::COMBAT) {
            SetState(GameState::PAUSED);
        } else if (currentState == GameState::PAUSED) {
            SetState(GameState::EXPLORING);
        }
    }
}

void Game::Update() {
    if (currentState == GameState::PAUSED) return;
    
    // 1. Update player
    player->Update(window->deltaTime);
    
    // 2. Apply physics
    physics.ApplyGravity(player->camera.Position, window->deltaTime);
    
    // 3. Floor collision
    int px = (int)std::floor(player->camera.Position.x);
    int py = (int)std::floor(player->camera.Position.y - 2.0f);
    int pz = (int)std::floor(player->camera.Position.z);
    
    Floor* currentFloor = floorManager.GetCurrentFloorPtr();
    if (currentFloor) {
        for (int dy = 0; dy <= 4; dy++) {
            if (currentFloor->GetBlock(px, py - dy, pz) != BLOCK_AIR) {
                float floorY = (float)(py - dy + 1) + 2.0f;
                physics.CheckFloor(player->camera.Position, floorY);
                break;
            }
        }
    }
    
    // Jump - higher jump when sprinting
    if (InputHandler::OnKeyPress(GLFW_KEY_SPACE) && physics.isGrounded) {
        float jumpVel = InputHandler::IsSprinting() ? 9.0f : 7.0f;
        physics.velocityY = jumpVel;
        physics.isGrounded = false;
    }
    
    // 4. Update world
    floorManager.Update(player->camera.Position);
    
    // 5. Update quest system
    questManager.Update(window->deltaTime);
    questManager.TrackLocation(player->camera.Position);
    
    // Check for completed quests to reward
    auto activeQuests = questManager.GetActiveQuests();
    for (auto* quest : activeQuests) {
        if (quest->IsCompleted()) {
            quest->CollectRewards(*player);
            std::cout << "Quest Completed: " << quest->title << std::endl;
        }
    }
    
    // 6. Update combat
    combat.Update(window->deltaTime);

    // 7. Apply player attacks (hit detection)
    HandleCombat();
    
    // 8. Update monsters
    UpdateMonsters();

    // 9. Update lock-on camera bias + third-person transitions
    HandleLockOn(window->deltaTime);
    
    // 10. Check combat state
    if (combat.IsSkillActive()) {
        SetState(GameState::COMBAT);
    } else {
        // Check if any monster is aggroed
        bool inCombat = false;
        for (auto& monster : monsters) {
            if (monster->state == MonsterState::CHASING || monster->state == MonsterState::ATTACKING) {
                inCombat = true;
                break;
            }
        }
        if (!inCombat && currentState == GameState::COMBAT) {
            SetState(GameState::EXPLORING);
        }
    }
    
    // 11. Check player death
    if (!player->IsAlive()) {
        OnPlayerDeath();
    }
}

void Game::Render() {
    // Clear with deep blue sky
    window->clear(0.2f, 0.4f, 0.8f);
    
    // Get shader
    Shader* blockShader = ResourceManager::GetShader("block");
    if (blockShader && player) {
        // Render world
        floorManager.Render(*blockShader, player->camera);
    }
    
    // HUD overlay (lock-on reticle + target HP bar)
    DrawLockOnHUD();

    // TODO: Render monsters, damage numbers, etc.
}

void Game::SpawnMonsters() {
    // Spawn some initial monsters around the spawn area
    std::vector<MonsterType> types = {
        MonsterType::WOLF,
        MonsterType::GOBLIN,
        MonsterType::SKELETON
    };
    
    for (int i = 0; i < 10; i++) {
        float angle = (float)i * 3.14159f * 2.0f / 10.0f;
        float radius = 20.0f + (float)(rand() % 30);
        glm::vec3 pos(cos(angle) * radius, 12.0f, sin(angle) * radius);
        
        MonsterType type = types[rand() % types.size()];
        monsters.push_back(std::make_unique<Monster>(pos, type));
    }
}

void Game::UpdateMonsters() {
    for (auto& monster : monsters) {
        if (monster->IsAlive()) {
            monster->Update(window->deltaTime);
            monster->UpdateAI(window->deltaTime, player->camera.Position);
            
            // Check if monster can attack player
            if (monster->state == MonsterState::ATTACKING && monster->attackTimer <= 0.0f) {
                player->TakeDamage(monster->attack);
            }
        } else {
            // Monster just died — reward player once, then begin respawn cooldown
            player->GainEXP(monster->expReward);
            
            // Track quest progress
            std::string typeStr;
            switch(monster->type) {
                case MonsterType::WOLF: typeStr = "wolf"; break;
                case MonsterType::BOAR: typeStr = "boar"; break;
                case MonsterType::GOBLIN: typeStr = "goblin"; break;
                case MonsterType::ORC: typeStr = "orc"; break;
                case MonsterType::SKELETON: typeStr = "skeleton"; break;
                case MonsterType::SLIME: typeStr = "slime"; break;
                case MonsterType::BOSS: typeStr = "boss"; break;
            }
            questManager.TrackKillProgress(typeStr);

            // Begin respawn cooldown (sets respawning = true internally)
            monster->Respawn();
        }
    }
}

void Game::HandleCombat() {
    if (!pendingAttack.active || !player) return;

    if (pendingAttack.range <= 0.0f) {
        pendingAttack.active = false;
        return;
    }

    glm::vec3 dir = pendingAttack.direction;
    if (glm::length(dir) <= 0.0001f) {
        pendingAttack.active = false;
        return;
    }
    dir = glm::normalize(dir);

    const int hits = std::max(1, pendingAttack.hitsToApply);
    const float hitSpacing = 0.15f; // SAO-style multi-hit spacing along the swing direction

    // Approximate player attack origin height.
    const glm::vec3 baseOrigin = player->camera.Position + glm::vec3(0.0f, 1.5f, 0.0f);

    const glm::vec3 attackSizeBasic = glm::vec3(0.35f, 0.65f, 0.35f);
    const glm::vec3 attackSizeSkill = glm::vec3(0.6f, 1.0f, 0.6f);
    const glm::vec3 attackSize = pendingAttack.isSkill ? attackSizeSkill : attackSizeBasic;

    float perHitDamage = pendingAttack.damage / (float)hits;
    if (perHitDamage < 0.0f) perHitDamage = 0.0f;

    // Apply damage.
    for (int hitIndex = 0; hitIndex < hits; hitIndex++) {
        glm::vec3 origin = baseOrigin + dir * (float)hitIndex * hitSpacing;

        for (auto& monsterPtr : monsters) {
            Monster* monster = monsterPtr.get();
            if (!monster || !monster->IsAlive()) continue;

            glm::vec3 aimPoint = monster->hitbox.center + glm::vec3(0.0f, monster->hitbox.size.y * 0.6f, 0.0f);
            glm::vec3 diff = aimPoint - origin;
            float forwardDist = glm::dot(diff, dir);

            if (forwardDist < 0.0f || forwardDist > pendingAttack.range) continue;

            glm::vec3 closestPoint = origin + dir * forwardDist;

            Hitbox attackBox;
            attackBox.center = glm::vec3(closestPoint.x, aimPoint.y, closestPoint.z);
            attackBox.size = attackSize;
            attackBox.isActive = true;

            if (!attackBox.Intersects(monster->hitbox)) continue;

            bool isCritical =
                (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) < player->criticalHitChance;
            float dmg = perHitDamage * (isCritical ? 1.5f : 1.0f);

            monster->TakeDamage(dmg);
            combat.AddDamageNumber(aimPoint, dmg, isCritical);
        }
    }

    pendingAttack.active = false;
}

void Game::QueuePlayerAttack(bool isSkill, const glm::vec3& direction, float range, float damage, int hitsToApply) {
    if (!player) return;
    if (combat.IsOnCooldown() || combat.IsSkillActive()) return; // match CombatSystem's acceptance conditions

    pendingAttack.active = true;
    pendingAttack.isSkill = isSkill;
    pendingAttack.hitsToApply = std::max(1, hitsToApply);
    pendingAttack.direction = direction;
    pendingAttack.range = range;
    pendingAttack.damage = damage;
}

void Game::ToggleLockOn() {
    if (lockOnEnabled) {
        lockOnEnabled = false;
        lockedTarget = nullptr;
        std::cout << "Lock-on disabled" << std::endl;
        return;
    }

    Monster* candidate = AcquireLockTarget();
    if (!candidate) {
        std::cout << "No lock-on target in range" << std::endl;
        return;
    }

    lockOnEnabled = true;
    lockedTarget = candidate;
    std::cout << "Locked onto: " << lockedTarget->name << std::endl;
}

bool Game::IsValidLockTarget(const Monster* target) const {
    if (!player || !target) return false;
    if (!target->IsAlive()) return false;

    glm::vec3 toTarget = GetLockAimPoint(target) - player->camera.Position;
    float dist = glm::length(toTarget);
    if (dist <= 0.0001f) return false;

    if (dist > lockOnMaxDistance * 1.1f) return false; // small persistence slack

    glm::vec3 dirToTarget = toTarget / dist;
    float dot = glm::dot(player->camera.Front, dirToTarget);

    // Allow slightly wider cone while locked, to avoid jitter.
    float maxAngleRad = (lockOnMaxAngleDegrees * 1.25f) * 0.01745329251f;
    float cosMaxAngle = std::cos(maxAngleRad);
    if (dot < cosMaxAngle) return false;

    // Occlusion check: ensure line of sight through the block world.
    const glm::vec3 eye = player->camera.Position + glm::vec3(0.0f, 1.5f, 0.0f);
    const glm::vec3 aimPoint = GetLockAimPoint(target);
    return HasLineOfSight(eye, aimPoint);
}

Monster* Game::AcquireLockTarget() {
    if (!player) return nullptr;

    auto candidates = GetValidLockCandidates();
    if (candidates.empty()) return nullptr;
    return candidates.front(); // candidates are pre-sorted by "most center"
}

bool Game::HasLineOfSight(const glm::vec3& from, const glm::vec3& to) const {
    const Floor* floor = floorManager.GetCurrentFloorPtr();
    if (!floor) return true;

    glm::vec3 delta = to - from;
    float dist = glm::length(delta);
    if (dist <= 0.0001f) return false;

    glm::vec3 dir = delta / dist;

    // Ray-march in small steps through the block grid.
    // Skip the first/last segments to avoid colliding with the target itself.
    const float step = 0.5f;
    const float startSkip = 0.25f;
    const float endSkip = 0.35f;

    for (float t = 0.0f; t < dist; t += step) {
        if (t < startSkip || (dist - t) < endSkip) continue;

        glm::vec3 p = from + dir * t;
        int bx = (int)std::floor(p.x);
        int by = (int)std::floor(p.y);
        int bz = (int)std::floor(p.z);

        if (floor->GetBlock(bx, by, bz) != BLOCK_AIR) {
            return false;
        }
    }

    return true;
}

std::vector<Monster*> Game::GetValidLockCandidates() const {
    std::vector<Monster*> candidates;
    if (!player) return candidates;

    for (auto& monsterPtr : monsters) {
        Monster* candidate = monsterPtr.get();
        if (!candidate) continue;
        if (!candidate->IsAlive()) continue;
        if (!IsValidLockTarget(candidate)) continue;
        candidates.push_back(candidate);
    }

    // Sort by best centered target (highest dot).
    const glm::vec3 camFront = player->camera.Front;
    const glm::vec3 camPos = player->camera.Position;
    std::sort(candidates.begin(), candidates.end(),
        [&](const Monster* a, const Monster* b) -> bool {
            glm::vec3 toA = GetLockAimPoint(a) - camPos;
            glm::vec3 toB = GetLockAimPoint(b) - camPos;
            float distA = glm::length(toA);
            float distB = glm::length(toB);
            if (distA <= 0.0001f) return false;
            if (distB <= 0.0001f) return true;
            glm::vec3 dirA = toA / distA;
            glm::vec3 dirB = toB / distB;
            float dotA = glm::dot(camFront, dirA);
            float dotB = glm::dot(camFront, dirB);
            return dotA > dotB;
        });

    return candidates;
}

void Game::CycleLockOnTarget(int direction) {
    if (!lockOnEnabled || !player) return;
    if (direction == 0) return;

    auto candidates = GetValidLockCandidates();
    if (candidates.empty()) {
        lockOnEnabled = false;
        lockedTarget = nullptr;
        std::cout << "Lock-on lost target" << std::endl;
        return;
    }

    int currentIndex = -1;
    for (int i = 0; i < (int)candidates.size(); i++) {
        if (candidates[i] == lockedTarget) {
            currentIndex = i;
            break;
        }
    }

    if (currentIndex < 0) {
        lockedTarget = candidates.front();
        std::cout << "Locked onto: " << lockedTarget->name << std::endl;
        return;
    }

    int n = (int)candidates.size();
    int nextIndex = (currentIndex + direction + n) % n;
    lockedTarget = candidates[nextIndex];
    if (lockedTarget) {
        std::cout << "Locked onto: " << lockedTarget->name << std::endl;
    }
}

glm::vec3 Game::GetLockAimPoint(const Monster* target) const {
    if (!target) return glm::vec3(0.0f);
    // Aim around chest/upper-body region using the monster hitbox half-height.
    return target->hitbox.center + glm::vec3(0.0f, target->hitbox.size.y * 0.6f, 0.0f);
}

void Game::HandleLockOn(float deltaTime) {
    if (!player) return;

    // Smoothly blend third-person view offset based on lock-on state.
    float targetBlend = lockOnEnabled ? 1.0f : 0.0f;
    float tBlend = 1.0f - std::exp(-thirdPersonResponse * deltaTime);
    thirdPersonBlend += (targetBlend - thirdPersonBlend) * tBlend;
    thirdPersonBlend = std::max(0.0f, std::min(1.0f, thirdPersonBlend));

    if (lockOnEnabled) {
        // Keep lock stable; reacquire if the current target is no longer valid.
        if (!lockedTarget || !IsValidLockTarget(lockedTarget)) {
            lockedTarget = AcquireLockTarget();
            if (!lockedTarget) {
                lockOnEnabled = false;
                std::cout << "Lock-on lost target" << std::endl;
            } else {
                std::cout << "Locked onto: " << lockedTarget->name << std::endl;
            }
        }

        if (lockedTarget && lockedTarget->IsAlive()) {
            glm::vec3 aimPoint = GetLockAimPoint(lockedTarget);
            glm::vec3 toTarget = aimPoint - player->camera.Position;
            float dist = glm::length(toTarget);

            if (dist > 0.0001f) {
                glm::vec3 dir = toTarget / dist;

                constexpr float RAD2DEG = 57.29577951308232f;
                float clampedY = std::max(-1.0f, std::min(1.0f, dir.y));
                float desiredPitch = std::asin(clampedY) * RAD2DEG;
                float desiredYaw = std::atan2(dir.z, dir.x) * RAD2DEG;

                auto NormalizeAngleDegrees = [](float a) -> float {
                    while (a > 180.0f) a -= 360.0f;
                    while (a < -180.0f) a += 360.0f;
                    return a;
                };

                float yawDiff = NormalizeAngleDegrees(desiredYaw - player->camera.Yaw);
                float pitchDiff = desiredPitch - player->camera.Pitch;

                float t = 1.0f - std::exp(-lockOnAssistResponse * deltaTime);
                player->camera.Yaw += yawDiff * t;
                player->camera.Pitch += pitchDiff * t;

                // Recompute Front/Right/Up from updated yaw/pitch.
                player->camera.ProcessMouseMovement(0.0f, 0.0f);
            }
        }
    }

    // Compute third-person camera offset based on current view direction.
    // Uses XZ forward so the camera doesn't "orbit" vertically when looking up/down.
    glm::vec3 frontXZ(player->camera.Front.x, 0.0f, player->camera.Front.z);
    if (glm::length(frontXZ) <= 0.0001f) {
        frontXZ = glm::vec3(0.0f, 0.0f, -1.0f);
    } else {
        frontXZ = glm::normalize(frontXZ);
    }

    glm::vec3 desiredOffset = (-frontXZ * thirdPersonDistance) + glm::vec3(0.0f, thirdPersonHeight, 0.0f);
    glm::vec3 finalOffset = glm::mix(glm::vec3(0.0f), desiredOffset, thirdPersonBlend);
    player->camera.SetViewOffset(finalOffset);
}

void Game::HandleFloorTransition() {
    int nextFloor = floorManager.GetCurrentFloor() + 1;
    if (nextFloor <= floorManager.GetTotalFloors()) {
        floorManager.ChangeFloor(nextFloor);
        
        // Move player to new floor spawn
        glm::vec3 spawn = floorManager.GetFloorSpawnPoint(nextFloor);
        player->camera.Position = spawn;
        player->Position = spawn;
    }
}

void Game::OnPlayerDeath() {
    std::cout << "GAME OVER - Player has died!" << std::endl;
    SetState(GameState::GAME_OVER);
    
    // Respawn player
    glm::vec3 spawn = floorManager.GetFloorSpawnPoint(floorManager.GetCurrentFloor());
    player->Reset(spawn);
    SetState(GameState::EXPLORING);
}
void Game::DrawLockOnHUD() {
    if (!lockOnEnabled || !lockedTarget || !lockedTarget->IsAlive() || !player) {
        return;
    }

    // 1. Get the 3D position (the aim point we defined earlier)
    glm::vec3 targetPos = GetLockAimPoint(lockedTarget);

    // 2. Math for 3D to 2D Projection
    float aspect = 1280.0f / 720.0f; 
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
    glm::mat4 view = player->camera.GetViewMatrix();

    glm::vec4 clipSpacePos = projection * view * glm::vec4(targetPos, 1.0f);
    if (clipSpacePos.w <= 0.0f) return;

    glm::vec3 ndc = glm::vec3(clipSpacePos) / clipSpacePos.w;

    // 3. Map to screen pixels
    float screenX = ((ndc.x + 1.0f) * 0.5f) * 1280.0f;
    float screenY = ((1.0f - ndc.y) * 0.5f) * 720.0f;

    // 4. Draw HUD elements using HUDRenderer
    hudRenderer.Begin(1280, 720);
    
    // SAO-style orange diamond reticle - larger and more visible
    glm::vec3 orangeColor(1.0f, 0.5f, 0.0f);
    hudRenderer.DrawDiamond(screenX, screenY, 25.0f, orangeColor);
    
    // Health bar above the diamond
    float hpRatio = lockedTarget->currentHP / lockedTarget->maxHP;
    hpRatio = std::max(0.0f, std::min(1.0f, hpRatio));
    
    glm::vec3 bgColor(0.2f, 0.2f, 0.2f);
    glm::vec3 healthColor;
    if (hpRatio > 0.5f) healthColor = glm::vec3(0.0f, 1.0f, 0.0f);      // Green
    else if (hpRatio > 0.2f) healthColor = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow
    else healthColor = glm::vec3(1.0f, 0.0f, 0.0f);                       // Red
    
    hudRenderer.DrawHealthBar(screenX, screenY - 50.0f, 100.0f, 12.0f, hpRatio, bgColor, healthColor);
    
    hudRenderer.End();
}
