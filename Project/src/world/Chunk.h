// Forward declaration
class Shader;

#pragma once
#include "Biome.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// Block type IDs
enum BlockType : uint8_t {
    BLOCK_AIR   = 0,
    BLOCK_GRASS = 1,
    BLOCK_DIRT  = 2,
    BLOCK_STONE = 3
};

// Size of one chunk in blocks
static const int CHUNK_SIZE = 16;


class Chunk {
public:
    Chunk(glm::ivec3 position);
    ~Chunk();

    void setBiome(const Biome& biome);
    Biome getBiome() const { return currentBiome; }

    // Get/set a block (local coords 0..CHUNK_SIZE-1)
    uint8_t GetBlock(int x, int y, int z) const;
    void    SetBlock(int x, int y, int z, uint8_t type);

    // Rebuild the greedy mesh from block data
    // Neighbor chunk pointers: nx/px/ny/py/nz/pz relative to this chunk
    void BuildMesh(const Chunk* nx, const Chunk* px, const Chunk* ny, const Chunk* py, const Chunk* nz, const Chunk* pz);

    // World position of this chunk (in chunk coordinates, not block coordinates)
    glm::ivec3 chunkPos;

    // The 3D grid of block types
    uint8_t blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE];

    // GPU handles
    unsigned int VAO = 0, VBO = 0, EBO = 0;
    int vertexCount = 0;

    void Draw(Shader& shader, int biomeType) const;

    bool isDirty = true; // Needs mesh rebuild?

private:
    Biome currentBiome;
};
