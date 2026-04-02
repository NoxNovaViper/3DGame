# 🗡️ SAO Game — AI Context Folder

This folder is the **"External RAM"** for any AI assistant helping with this project.
Paste the relevant `.md` files from here into your AI chat (or the system prompt) at the start of every session so the model never forgets your architecture, naming rules, or what's already been built.

---

## 📁 Folder Structure

```
./
├── README .md             ← You are here. Read this first.
├── project-context.md     ← Main context file — paste this into the AI every session
├── architecture.md        ← Engine/renderer design overview (update as you build)
├── features.md            ← What's done, what's in progress, what's planned
└── bugs.md                ← Known issues the AI should NOT reintroduce
```

---

## 🚀 How to Use

### Starting a new AI session
1. Open `project-context.md`
2. Copy the **entire file**
3. Paste it as your **first message** (or into the system prompt if using an API/local model)
4. The AI now knows your project — start coding

### Keeping it up to date
After each session where something significant was built or changed:
- Add new classes/systems to `architecture.md`
- Move finished features from "Planned" → "Done" in `features.md`
- Log any repeating AI mistakes in `bugs.md`

---

## 📋 Context Template Format

Each context file should follow this structure so any AI model can parse it reliably:

```markdown
# Project Context: SAO Game
**Stack:** OpenGL | GLM | GLAD | GLFW | C++

## 1. Architecture Constraints
- **Language:** C++17
- **Window/Input:** GLFW
- **OpenGL Loader:** GLAD
- **Math Library:** GLM (glm::vec3, glm::mat4 etc — never raw arrays)
- **Shader location:** src/shaders/
- **Entry point:** main.cpp → Game::run()

## 2. Naming & Code Standards
- Classes: PascalCase         → Renderer, PlayerController, MeshLoader
- Files:   snake_case.cpp/h   → mesh_loader.cpp, player_controller.h
- Shaders: name.vert / name.frag
- Uniforms: camelCase         → modelMatrix, lightPos
- Never use `using namespace std` in headers
- Always include glad.h BEFORE glfw3.h

## 3. Current Systems (DO NOT rewrite unless asked)
- Window init via GLFW ✅
- GLAD loader setup ✅
- Basic render loop ✅
- [add more as you build]

## 4. Planned Features
- Player movement (SAO-style lock-on combat)
- Skybox / environment
- HUD (health bar, stamina ring)
- [add your goals here]

## 5. Known Issues / AI Mistakes to Avoid
- Do NOT use deprecated OpenGL (glBegin/glEnd etc)
- Do NOT use GLUT — project uses GLFW only
- Do NOT forget to call gladLoadGLLoader before any GL calls
- [log new mistakes here as they happen]
```

---

## 🧠 Why This Works

Local and "dumb" models have a short memory — they forget your project structure mid-conversation and start hallucinating wrong class names, wrong headers, or deprecated APIs.

By feeding them this file upfront you get:

| Problem | Fix |
|---|---|
| AI invents random class names | Lock them in `## Architecture Constraints` |
| AI uses wrong OpenGL version | Specify it explicitly |
| AI rewrites code you already have | List it in `## Current Systems` |
| AI repeats the same bugs | Log them in `## Known Issues` |

---

## ⚙️ Stack Reference (quick copy-paste for prompts)

```
C++17 | OpenGL 3.3 Core | GLFW 3 | GLAD | GLM
Shader path: src/shaders/
Build: CMake (or specify yours)
```

---

*Update this folder as the project grows. The more specific it is, the smarter your AI gets.*
