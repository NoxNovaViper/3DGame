#pragma once

// Forward declarations
class Shader;
class Camera;

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "world/ChunkManager.h"
#include "world/Chunk.h"

struct FloorConfig {
    int floorNumber;
    std::string name;
    float radius;       // Floor radius in blocks
    int maxHeight;      // Max terrain height
    glm::vec3 skyColor; // Sky color for this floor
    glm::vec3 fogColor; // Fog color
    bool hasBoss = true;
    bool hasTown = true;
    bool hasDungeon = true;
};

class Floor {
public:
    FloorConfig config;
    ChunkManager chunkManager; // owned by value, not pointer

    Floor(int floorNumber, unsigned int seed);
    ~Floor();

    void Update(const glm::vec3& playerPos);
    void Render(Shader& shader, const Camera& camera);

    uint8_t GetBlock(int worldX, int worldY, int worldZ) const;
    void    SetBlock(int worldX, int worldY, int worldZ, uint8_t type);

    // Floor-specific generation
    void GenerateTerrain();

    // Check if position is within floor boundary
    bool IsWithinFloor(const glm::vec3& pos) const;

    // Get spawn point for this floor
    glm::vec3 GetSpawnPoint() const;

private:
    // Generate circular floor terrain
    void GenerateCircularTerrain(Chunk& chunk);

    // Generate town area in center
    void GenerateTownArea(Chunk& chunk, int cx, int cz);

    // Generate forest/field areas
    void GenerateForestArea(Chunk& chunk, int cx, int cz);

    // Generate dungeon area
    void GenerateDungeonArea(Chunk& chunk, int cx, int cz);

    // Generate boundary walls
    void GenerateBoundaryWalls();

    // Generate ceiling
    void GenerateCeiling();

    float GetDistanceFromCenter(int x, int z) const;
};
