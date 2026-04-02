# SAO Features Status

## Implemented (current)
- GLFW window/context setup + main loop (`src/game/`)
- Camera input (mouse look) + WASD movement (`src/core/InputHandler.*`, `src/core/Camera.*`)
- Chunk-based world streaming (`ChunkManager`) and biome selection wiring (`WorldRenderer`)
- SAO-style lock-on targeting + camera aim assist (forward-cone selection + smooth yaw/pitch bias)
- Third-person lock-on camera transitions (view-offset blend)
- Entity base class + entities (`src/entities/`):
  - `Player` (stats, inventory scaffolding, blocking state)
  - `Monster` (simple AI + hitbox)
  - `NPC` (dialog + merchant inventory scaffolding)
- Combat system scaffolding:
  - `CombatSystem` handles cooldowns/skill timers and damage number bookkeeping (hit application now implemented via `Game::HandleCombat`)
- Quest scaffolding:
  - `QuestManager` tracks progress and rewards on quest completion

## Pending / Next (toward true 3D SAO feel)
- Replace voxel-like chunk/block rendering with mesh-based models (plan calls this out)
- Improve shaders (lighting/shadows), and address performance (texture loading, mesh rebuild rates)

