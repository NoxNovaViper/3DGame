# SAO Known Issues / Mistakes to Avoid

## Duplicated systems (fixed)
- `Player` used to allocate its own `CombatSystem` and `Physics` even though `Game` already owns authoritative instances.
- This has been corrected: `Game::Initialize` assigns pointers into the `Player`, and `Player` no longer updates the combat timer locally.

## Rendering pipeline issues (watch-outs)
- `Chunk` previously did not issue any draw call; `Chunk::Draw()` now renders via `glDrawArrays`.
- `Chunk::BuildMesh()` is a basic cube-per-face mesher right now (blocky look). It must evolve toward 3D SAO-style geometry.
- Texture loading in `WorldRenderer` currently loads biome textures frequently; this is a performance issue, not a correctness issue.

## Combat integration gaps
- Attack input triggers cooldowns and skills, and hit application is now handled in `Game::HandleCombat` (approximate ray/volume vs monster `Hitbox`).

## AI mistakes to avoid
- Do not introduce duplicate authoritative systems/timers into `Player` when `Game` already owns them.
- Do not use deprecated OpenGL features (Core 3.3 only).
- Keep `#include` paths rooted correctly under `src/` (CMake include root).

