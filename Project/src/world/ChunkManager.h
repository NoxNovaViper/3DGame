#pragma once
#include "world/Chunk.h"
#include "world/Noise.h"
#include "world/Biome.h"
#include "core/Shader.h"
#include "core/Camera.h"
#include <unordered_map>
#include <glm/glm.hpp>

static const int RENDER_DISTANCE = 6;

struct IVec3Hash {
    size_t operator()(const glm::ivec3& v) const {
        return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1) ^ (std::hash<int>()(v.z) << 2);
    }
};

struct IVec3Equal {
    bool operator()(const glm::ivec3& a, const glm::ivec3& b) const {
        return a.x == b.x && a.y == b.y && a.z == b.z;
    }
};

class ChunkManager {
public:
    ChunkManager(unsigned int worldSeed = 12345);
    ~ChunkManager();

    void Update(const glm::vec3& playerPos);
    void Render(Shader& shader, const Camera& camera, int biomeType);
    const std::unordered_map<glm::ivec3, Chunk*, IVec3Hash, IVec3Equal>& GetChunks() const;

    static glm::ivec3 WorldToChunk(const glm::vec3& worldPos);
    uint8_t GetBlock(int worldX, int worldY, int worldZ) const;
    void    SetBlock(int worldX, int worldY, int worldZ, uint8_t type);
    Chunk* GetChunk(glm::ivec3 pos) const;
    Chunk* LoadChunk(glm::ivec3 chunkPos);
    void UnloadChunk(glm::ivec3 chunkPos);
    void RebuildDirtyChunks();

private:
    std::unordered_map<glm::ivec3, Chunk*, IVec3Hash, IVec3Equal> chunks;
    Noise worldGen;
};