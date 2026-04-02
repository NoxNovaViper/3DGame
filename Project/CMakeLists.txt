cmake_minimum_required(VERSION 3.10)
project(SAO_Aincrad)

set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(
    src
    libraries/glfw/include
    libraries/glad/include
    libraries/glm
)

# GLAD source
set(GLAD_SRC libraries/glad/src/glad.c)

# Find GLFW
set(GLFW_LIB C:/Users/HP/Downloads/mcoriject/libraries/glfw/lib-mingw-w64/libglfw3.a)

# Core source files
set(CORE_SOURCES
    src/game/main.cpp
    src/core/Camera.cpp
    src/core/Shader.cpp
    src/core/InputHandler.cpp
    src/core/ColorHandler.cpp
    src/core/Mesh.cpp
    src/core/Window.cpp
    src/core/HUDRenderer.cpp
    src/world/WorldRenderer.cpp
    src/core/Physics.cpp
    src/core/ResourceManager.cpp
    src/world/Chunk.cpp
    src/world/ChunkManager.cpp
)

# Game system source files
set(GAME_SOURCES
    src/game/Game.cpp
    src/entities/Entity.cpp
    src/entities/Player.cpp
    src/entities/Monster.cpp
    src/entities/NPC.cpp
    src/combat/SwordSkill.cpp
    src/combat/Skill.cpp
    src/combat/Hitbox.cpp
    src/combat/CombatSystem.cpp
    src/combat/HealingPotion.cpp
    src/world/Floor.cpp
    src/world/FloorManager.cpp
    src/quest/Quest.cpp
    src/quest/QuestManager.cpp
)

# Add executable
add_executable(SAO_Aincrad 
    ${CORE_SOURCES}
    ${GAME_SOURCES}
    ${GLAD_SRC}
)

# Link libraries
target_link_libraries(SAO_Aincrad
    ${GLFW_LIB}
    opengl32
    gdi32
    user32
    shell32
)
