#include "Floor.h"
#include "Noise.h"
#include "Biome.h"
#include <cmath>
#include <algorithm>

static FloorConfig GetFloorConfig(int floorNumber) {
    FloorConfig config;
    config.floorNumber = floorNumber;
    
    switch (floorNumber) {
        case 1:
            config.name = "Town of Beginnings";
            config.radius = 100.0f;
            config.maxHeight = 20;
            config.skyColor = glm::vec3(0.53f, 0.81f, 0.98f);
            config.fogColor = glm::vec3(0.7f, 0.85f, 0.95f);
            break;
        case 2:
            config.name = "First Floor - Forest";
            config.radius = 120.0f;
            config.maxHeight = 30;
            config.skyColor = glm::vec3(0.4f, 0.7f, 0.9f);
            config.fogColor = glm::vec3(0.6f, 0.8f, 0.7f);
            break;
        case 3:
            config.name = "Second Floor - Canyon";
            config.radius = 140.0f;
            config.maxHeight = 40;
            config.skyColor = glm::vec3(0.8f, 0.6f, 0.4f);
            config.fogColor = glm::vec3(0.7f, 0.6f, 0.5f);
            break;
        default:
            config.name = "Floor " + std::to_string(floorNumber);
            config.radius = 100.0f + floorNumber * 10.0f;
            config.maxHeight = 20 + floorNumber * 5;
            config.skyColor = glm::vec3(0.5f, 0.7f, 0.9f);
            config.fogColor = glm::vec3(0.6f, 0.7f, 0.8f);
            break;
    }
    
    return config;
}

Floor::Floor(int floorNumber, unsigned int seed) 
    : config(GetFloorConfig(floorNumber)), chunkManager(seed) 
{
    GenerateTerrain();
}

Floor::~Floor() {}

void Floor::Update(const glm::vec3& playerPos) {
    chunkManager.Update(playerPos);
}

void Floor::Render(Shader& shader, const Camera& camera) {
    // Map floor number to a biome type for shader/uniform selection.
    int biomeType = static_cast<int>(BiomeType::FOREST);
    switch (config.floorNumber) {
        case 1: biomeType = static_cast<int>(BiomeType::FOREST); break;
        case 2: biomeType = static_cast<int>(BiomeType::DESERT); break;
        case 3: biomeType = static_cast<int>(BiomeType::MOUNTAIN); break;
        default: biomeType = static_cast<int>(BiomeType::OCEAN); break;
    }
    chunkManager.Render(shader, camera, biomeType);
}

uint8_t Floor::GetBlock(int worldX, int worldY, int worldZ) const {
    return chunkManager.GetBlock(worldX, worldY, worldZ);
}

void Floor::SetBlock(int worldX, int worldY, int worldZ, uint8_t type) {
    chunkManager.SetBlock(worldX, worldY, worldZ, type);
}

void Floor::GenerateTerrain() {
    // Terrain is generated on-demand by ChunkManager
}

bool Floor::IsWithinFloor(const glm::vec3& pos) const {
    float dist = GetDistanceFromCenter((int)pos.x, (int)pos.z);
    return dist < config.radius;
}

glm::vec3 Floor::GetSpawnPoint() const {
    return glm::vec3(0.0f, (float)config.maxHeight + 5.0f, 0.0f);
}

void Floor::GenerateCircularTerrain(Chunk& chunk) {
    // Override the default noise-based generation with circular floor layout
    int ox = chunk.chunkPos.x * CHUNK_SIZE;
    int oz = chunk.chunkPos.z * CHUNK_SIZE;
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int gx = ox + x;
            int gz = oz + z;
            
            float dist = GetDistanceFromCenter(gx, gz);
            
            for (int y = 0; y < CHUNK_SIZE; y++) {
                int gy = chunk.chunkPos.y * CHUNK_SIZE + y;
                
                // Outside floor radius = void (air)
                if (dist > config.radius) {
                    chunk.SetBlock(x, y, z, BLOCK_AIR);
                    continue;
                }
                
                // Base terrain height with slight variation
                int baseHeight = config.maxHeight / 2;
                float edgeFalloff = 1.0f - (dist / config.radius);
                int height = (int)(baseHeight * edgeFalloff);
                
                if (gy < height - 3) {
                    chunk.SetBlock(x, y, z, BLOCK_STONE);
                } else if (gy < height) {
                    chunk.SetBlock(x, y, z, BLOCK_DIRT);
                } else if (gy == height) {
                    chunk.SetBlock(x, y, z, BLOCK_GRASS);
                } else {
                    chunk.SetBlock(x, y, z, BLOCK_AIR);
                }
            }
        }
    }
}

void Floor::GenerateTownArea(Chunk& chunk, int cx, int cz) {
    // Town is in the center of the floor
    // For now, keep the terrain flat in the center area
    int ox = chunk.chunkPos.x * CHUNK_SIZE;
    int oz = chunk.chunkPos.z * CHUNK_SIZE;
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int gx = ox + x;
            int gz = oz + z;
            float dist = GetDistanceFromCenter(gx, gz);
            
            if (dist < 20.0f) {
                // Flat town area
                int townHeight = 10;
                for (int y = 0; y < CHUNK_SIZE; y++) {
                    int gy = chunk.chunkPos.y * CHUNK_SIZE + y;
                    if (gy < townHeight - 1) {
                        chunk.SetBlock(x, y, z, BLOCK_STONE);
                    } else if (gy == townHeight) {
                        chunk.SetBlock(x, y, z, BLOCK_GRASS);
                    } else {
                        chunk.SetBlock(x, y, z, BLOCK_AIR);
                    }
                }
            }
        }
    }
}

void Floor::GenerateForestArea(Chunk& chunk, int cx, int cz) {
    // Forest areas are at medium distance from center
    // Trees will be added later as separate entities
}

void Floor::GenerateDungeonArea(Chunk& chunk, int cx, int cz) {
    // Dungeon is at the edge of the floor
    // Will be implemented with special block types
}

void Floor::GenerateBoundaryWalls() {
    // Invisible walls at floor boundary to prevent falling
    // This is handled by IsWithinFloor check
}

void Floor::GenerateCeiling() {
    // Ceiling at a certain height above the floor
    // Will be implemented later
}

float Floor::GetDistanceFromCenter(int x, int z) const {
    return std::sqrt((float)(x * x + z * z));
}
