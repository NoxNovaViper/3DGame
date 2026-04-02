#include "world/Chunk.h"
#include "core/Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <cstring>
#include <vector>

Chunk::Chunk(glm::ivec3 pos) : chunkPos(pos), currentBiome(Biome(BiomeType::FOREST, "textures/forest.png", 0.5f, 0.0f)) {
    memset(blocks, BLOCK_AIR, sizeof(blocks));
}

Chunk::~Chunk() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void Chunk::setBiome(const Biome& biome) {
    currentBiome = biome;
}

uint8_t Chunk::GetBlock(int x, int y, int z) const {
    return blocks[x][y][z];
}

void Chunk::SetBlock(int x, int y, int z, uint8_t type) {
    blocks[x][y][z] = type;
}

void Chunk::Draw(Shader& shader, int biomeType) const {
    shader.setBiomeType("biomeType", biomeType);
    if (vertexCount <= 0 || VAO == 0) return;
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void Chunk::BuildMesh(const Chunk* nx, const Chunk* px, const Chunk* ny, const Chunk* py, const Chunk* nz, const Chunk* pz) {
    // Build a simple per-face cube mesh for all non-air blocks.
    // Vertex layout must match `core/Shader.h` (aPos=0, aNormal=1, aTexCoord=2).
    std::vector<float> vertices;
    vertices.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 36 * 8); // rough upper bound

    auto pushVertex = [&](float x, float y, float z, float nx_, float ny_, float nz_, float u, float v) {
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(z);
        vertices.push_back(nx_);
        vertices.push_back(ny_);
        vertices.push_back(nz_);
        vertices.push_back(u);
        vertices.push_back(v);
    };

    auto pushQuad = [&](glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 n) {
        // Two triangles: (v0, v1, v2) and (v0, v2, v3)
        // Texcoords mapped consistently to face corners.
        pushVertex(v0.x, v0.y, v0.z, n.x, n.y, n.z, 0.0f, 0.0f);
        pushVertex(v1.x, v1.y, v1.z, n.x, n.y, n.z, 1.0f, 0.0f);
        pushVertex(v2.x, v2.y, v2.z, n.x, n.y, n.z, 1.0f, 1.0f);

        pushVertex(v0.x, v0.y, v0.z, n.x, n.y, n.z, 0.0f, 0.0f);
        pushVertex(v2.x, v2.y, v2.z, n.x, n.y, n.z, 1.0f, 1.0f);
        pushVertex(v3.x, v3.y, v3.z, n.x, n.y, n.z, 0.0f, 1.0f);
    };

    auto getBlock = [&](int x, int y, int z) -> uint8_t {
        if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE && z >= 0 && z < CHUNK_SIZE) {
            return blocks[x][y][z];
        }

        // Out-of-bounds: query neighbor chunks (or treat missing neighbors as air).
        if (x < 0) {
            return nx ? nx->GetBlock(CHUNK_SIZE - 1, y, z) : BLOCK_AIR;
        }
        if (x >= CHUNK_SIZE) {
            return px ? px->GetBlock(0, y, z) : BLOCK_AIR;
        }
        if (y < 0) {
            return ny ? ny->GetBlock(x, CHUNK_SIZE - 1, z) : BLOCK_AIR;
        }
        if (y >= CHUNK_SIZE) {
            return py ? py->GetBlock(x, 0, z) : BLOCK_AIR;
        }
        if (z < 0) {
            return nz ? nz->GetBlock(x, y, CHUNK_SIZE - 1) : BLOCK_AIR;
        }
        if (z >= CHUNK_SIZE) {
            return pz ? pz->GetBlock(x, y, 0) : BLOCK_AIR;
        }
        return BLOCK_AIR;
    };

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                uint8_t blockType = blocks[x][y][z];
                if (blockType == BLOCK_AIR) continue;

                // Cube corners in local chunk space.
                float fx = (float)x;
                float fy = (float)y;
                float fz = (float)z;

                // Check each neighbor; if it's air, emit that face.
                // +X face
                if (getBlock(x + 1, y, z) == BLOCK_AIR) {
                    glm::vec3 n(1.0f, 0.0f, 0.0f);
                    glm::vec3 v0(fx + 1, fy, fz + 1);
                    glm::vec3 v1(fx + 1, fy, fz);
                    glm::vec3 v2(fx + 1, fy + 1, fz);
                    glm::vec3 v3(fx + 1, fy + 1, fz + 1);
                    pushQuad(v0, v1, v2, v3, n);
                }
                // -X face
                if (getBlock(x - 1, y, z) == BLOCK_AIR) {
                    glm::vec3 n(-1.0f, 0.0f, 0.0f);
                    glm::vec3 v0(fx, fy, fz);
                    glm::vec3 v1(fx, fy, fz + 1);
                    glm::vec3 v2(fx, fy + 1, fz + 1);
                    glm::vec3 v3(fx, fy + 1, fz);
                    pushQuad(v0, v1, v2, v3, n);
                }
                // +Y face
                if (getBlock(x, y + 1, z) == BLOCK_AIR) {
                    glm::vec3 n(0.0f, 1.0f, 0.0f);
                    glm::vec3 v0(fx, fy + 1, fz + 1);
                    glm::vec3 v1(fx + 1, fy + 1, fz + 1);
                    glm::vec3 v2(fx + 1, fy + 1, fz);
                    glm::vec3 v3(fx, fy + 1, fz);
                    pushQuad(v0, v1, v2, v3, n);
                }
                // -Y face
                if (getBlock(x, y - 1, z) == BLOCK_AIR) {
                    glm::vec3 n(0.0f, -1.0f, 0.0f);
                    glm::vec3 v0(fx, fy, fz);
                    glm::vec3 v1(fx + 1, fy, fz);
                    glm::vec3 v2(fx + 1, fy, fz + 1);
                    glm::vec3 v3(fx, fy, fz + 1);
                    pushQuad(v0, v1, v2, v3, n);
                }
                // +Z face
                if (getBlock(x, y, z + 1) == BLOCK_AIR) {
                    glm::vec3 n(0.0f, 0.0f, 1.0f);
                    glm::vec3 v0(fx, fy, fz + 1);
                    glm::vec3 v1(fx + 1, fy, fz + 1);
                    glm::vec3 v2(fx + 1, fy + 1, fz + 1);
                    glm::vec3 v3(fx, fy + 1, fz + 1);
                    pushQuad(v0, v1, v2, v3, n);
                }
                // -Z face
                if (getBlock(x, y, z - 1) == BLOCK_AIR) {
                    glm::vec3 n(0.0f, 0.0f, -1.0f);
                    glm::vec3 v0(fx + 1, fy, fz);
                    glm::vec3 v1(fx, fy, fz);
                    glm::vec3 v2(fx, fy + 1, fz);
                    glm::vec3 v3(fx + 1, fy + 1, fz);
                    pushQuad(v0, v1, v2, v3, n);
                }
            }
        }
    }

    // Upload to GPU.
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
    VAO = 0;
    VBO = 0;
    EBO = 0;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    const GLsizei stride = 8 * sizeof(float);
    // aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // aNormal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // aTexCoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    vertexCount = (int)(vertices.size() / 8);
    isDirty = false;
}
