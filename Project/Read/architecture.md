# SAO Engine Architecture

## Core constraints
- C++17 + OpenGL 3.3 Core profile
- GLFW for window/input, GLAD for loading, GLM for math
- Shader code is embedded in `src/core/Shader.h` (uniforms/attribute layout must match)
- Include path root: treat `src/` as the include root (use `#include "core/Camera.h"` style)

## High-level system layout
- `src/game/`: game loop (`Game::run`) + state transitions
- `src/core/`: rendering primitives (`Shader`, `Mesh`), window/input (`Window`, `InputHandler`), simulation (`Physics`)
- `src/world/`: terrain streaming + rendering (`Chunk`, `ChunkManager`, `Floor`, `FloorManager`, `WorldRenderer`)
- `src/entities/`: base `Entity` + `Player`, `Monster`, `NPC`
- `src/combat/`: `CombatSystem`, `SwordSkill`, `Hitbox` (basic attack/skill cooldowns exist; hit detection is pending)
- `src/quest/`: `Quest` + `QuestManager`
- `src/ui/`: placeholder UI header

## Ownership / authority rules
- `Game` owns the authoritative simulation/timers:
  - `CombatSystem combat`
  - `Physics physics`
- `Player` holds pointers (`Player::combatSystem`, `Player::physics`) that are assigned in `Game::Initialize` so `Player` methods can interact with the correct systems without duplicating them.

## Rendering contracts
- Chunk shaders expect a vertex format:
  - location 0: `aPos` (vec3)
  - location 1: `aNormal` (vec3)
  - location 2: `aTexCoord` (vec2)
- Chunk meshes are generated from block data in `Chunk::BuildMesh()` and drawn in `Chunk::Draw()`.

