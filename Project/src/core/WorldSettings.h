#pragma once
#include <glm/glm.hpp>

// Global world constants shared across all systems.
// Include as: #include "core/WorldSettings.h"

namespace WorldSettings {

    // ---- Chunk / terrain ----
    constexpr int   CHUNK_SIZE_BLOCKS = 16;       // matches CHUNK_SIZE in Chunk.h
    constexpr int   RENDER_DISTANCE_CHUNKS = 6;   // matches RENDER_DISTANCE in ChunkManager.h
    constexpr float BLOCK_SIZE = 1.0f;            // world units per block

    // ---- Floor layout ----
    constexpr int   TOTAL_FLOORS = 100;
    constexpr float DEFAULT_FLOOR_RADIUS = 100.0f;  // blocks from center
    constexpr float FLOOR_BOUNDARY_WALL_HEIGHT = 64.0f;
    constexpr float FLOOR_CEILING_HEIGHT = 128.0f;

    // ---- Physics ----
    constexpr float GRAVITY = -9.8f;
    constexpr float PLAYER_JUMP_FORCE = 7.0f;
    constexpr float PLAYER_MOVE_SPEED = 5.0f;
    constexpr float PLAYER_EYE_HEIGHT = 1.7f;      // camera offset from feet

    // ---- Combat ----
    constexpr float BASIC_ATTACK_RANGE = 2.5f;
    constexpr float BASIC_ATTACK_COOLDOWN = 0.5f;
    constexpr float MONSTER_AGGRO_RANGE = 12.0f;
    constexpr float MONSTER_ATTACK_RANGE = 2.0f;
    constexpr float MONSTER_RESPAWN_DELAY = 30.0f;  // seconds

    // ---- Rendering ----
    constexpr int   SCREEN_WIDTH  = 1280;
    constexpr int   SCREEN_HEIGHT = 720;
    constexpr float FOV_DEGREES   = 45.0f;
    constexpr float NEAR_PLANE    = 0.1f;
    constexpr float FAR_PLANE     = 500.0f;

    // ---- Lighting ----
    inline glm::vec3 SunDirection()  { return glm::vec3(0.4f, -1.0f, 0.3f); }
    inline glm::vec3 SunColor()      { return glm::vec3(1.0f, 0.95f, 0.8f); }
    inline glm::vec3 AmbientColor()  { return glm::vec3(0.15f, 0.15f, 0.2f); }

    // ---- Sky ----
    inline glm::vec3 DefaultSkyColor() { return glm::vec3(0.53f, 0.81f, 0.98f); }
    inline glm::vec3 NightSkyColor()   { return glm::vec3(0.02f, 0.02f, 0.08f); }

} // namespace WorldSettings
