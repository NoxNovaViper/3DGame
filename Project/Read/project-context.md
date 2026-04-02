# Project Context: SAO Game
**Stack:** C++17 | OpenGL 3.3 Core | GLFW 3 | GLAD | GLM

---

## 1. Architecture Constraints
- **Language:** C++17
- **Window & Input:** GLFW 3
- **OpenGL Loader:** GLAD — always `#include <glad/glad.h>` BEFORE `#include <GLFW/glfw3.h>`
- **Math:** GLM only — `glm::vec3`, `glm::mat4`, `glm::radians()` etc. Never raw float arrays for transforms
- **Entry point:** `src/game/main.cpp` → `Game::run()`
- **Build system:** CMake (`CMakeLists.txt` in root)
- **Libraries location:** `libraries/` (glad, glfw, glm) — ignored by Roo Code but present on disk
- **Build output:** `build/` — ignored by Roo Code

---

## 2. Folder Structure
```
project/
├── .gitignore
├── CMakeLists.txt
├── src/
│   ├── core/
│   │   ├── Camera.h / Camera.cpp
│   │   ├── Mesh.h / Mesh.cpp
│   │   ├── Physics.h / Physics.cpp
│   │   ├── Shader.h / Shader.cpp
│   │   ├── Window.h / Window.cpp
│   │   ├── ResourceManager.h / ResourceManager.cpp
│   │   └── WorldSettings.h
│   ├── world/
│   │   ├── Chunk.h / Chunk.cpp
│   │   ├── ChunkManager.h / ChunkManager.cpp
│   │   ├── Floor.h / Floor.cpp
│   │   ├── FloorManager.h / FloorManager.cpp
│   │   ├── WorldRenderer.h / WorldRenderer.cpp
│   │   └── Noise.h
│   ├── entities/
│   │   ├── Entity.h / Entity.cpp        ← base class
│   │   ├── Player.h / Player.cpp
│   │   ├── Monster.h / Monster.cpp
│   │   └── NPC.h / NPC.cpp
│   ├── combat/
│   │   ├── CombatSystem.h / CombatSystem.cpp
│   │   ├── Hitbox.h / Hitbox.cpp
│   │   └── SwordSkill.h / SwordSkill.cpp
│   ├── quest/
│   │   ├── Quest.h / Quest.cpp
│   │   └── QuestManager.h / QuestManager.cpp
│   ├── game/
│   │   ├── main.cpp
│   │   ├── Game.h / Game.cpp
│   │   └── GameState.h
│   └── ui/
│       └── UI.h
├── libraries/          ← hidden from Roo Code
│   ├── glad/
│   ├── glfw/
│   └── glm/
└── build/              ← hidden from Roo Code
```

---

## 3. Include Path Rules ⚠️
CMake sets `src/` as an include root. Always use full relative paths from `src/`:

```cpp
// CORRECT
#include "core/WorldSettings.h"
#include "world/FloorManager.h"
#include "entities/Entity.h"
#include "combat/CombatSystem.h"

// WRONG — will cause build errors
#include "WorldSettings.h"
#include "FloorManager.h"
```

Cross-folder includes must always specify the subfolder — e.g. `FloorManager.h` including `WorldSettings.h` must write `#include "core/WorldSettings.h"`.

---

## 4. Naming & Code Standards
- Classes:   `PascalCase`       → `ChunkManager`, `CombatSystem`, `SwordSkill`
- Files:     `PascalCase.cpp/h` → `FloorManager.cpp`, `WorldRenderer.h`
- Shaders:   `name.vert` / `name.frag`
- Uniforms:  `camelCase`        → `modelMatrix`, `viewMatrix`, `lightPos`
- **Never** `using namespace std;` in any header
- **Always** `#pragma once` in all headers
- **Never** `using namespace glm;` — always qualify (`glm::vec3` not `vec3`)

---

## 5. Current Systems ✅ (DO NOT rewrite unless explicitly asked)
- Camera (`src/core/Camera`)
- Mesh loading/rendering (`src/core/Mesh`)
- Shader compile/link/use (`src/core/Shader`)
- Window creation & GL context (`src/core/Window`)
- Resource management (`src/core/ResourceManager`)
- Physics (`src/core/Physics`)
- World generation — Chunk, ChunkManager, Floor, FloorManager, Noise (`src/world/`)
- World rendering (`src/world/WorldRenderer`) — **Biome-specific textures**
- Entity base class + Player, Monster, NPC (`src/entities/`)
- Combat — CombatSystem, Hitbox, SwordSkill (`src/combat/`)
- Quest + QuestManager (`src/quest/`)
- Game loop + GameState (`src/game/`)
- Basic HUD (`src/ui/UI.h`)

## 6. Biome-Specific Rendering

The biome-specific rendering pipeline has been integrated into the following components:

- **`WorldRenderer`** (`src/world/WorldRenderer.cpp`):
  - Updated `RenderChunkWithBiome` to render chunks with biome-specific textures.
  - Ensured `stb_image.h` is included for texture loading.

- **`ChunkManager`** (`src/world/ChunkManager.h`, `src/world/ChunkManager.cpp`):
  - Added methods: `GetChunk`, `LoadChunk`, `UnloadChunk`, `RebuildDirtyChunks`.
  - Initialized `worldGen` and updated the `Render` method for correct texture rendering.

- **`Chunk`** (`src/world/Chunk.h`, `src/world/Chunk.cpp`):
  - Modified the `Draw` method to support biome-specific rendering.

## 7. Known Issues & AI Mistakes to Avoid ⚠️
- Do NOT use deprecated OpenGL (`glBegin`, `glEnd`, `glVertex3f`) — Core 3.3 profile only
- Do NOT use GLUT — this project uses GLFW only
- Do NOT call any `gl*` function before `gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)`
- Do NOT write bare `#include "FloorManager.h"` — must be `#include "world/FloorManager.h"`
- Do NOT use `glm::perspective` with degrees — always wrap in `glm::radians()`
- **Do NOT pass a `Shader` object to `chunk.Draw()`** in biome-specific rendering mode.
- Do NOT invent new files or classes without being asked — all major systems already exist (see section 5)
- *(add new AI mistakes here as they happen)*
- Do NOT duplicate `CombatSystem`/`Physics` inside `Player` when `Game` already owns the authoritative systems.

---

## 6. Planned / In Progress 📋
- *(move items here and keep updated)*
