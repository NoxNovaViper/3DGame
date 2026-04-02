#include "ChunkManager.h"
#include "Biome.h"
#include "stb_image.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_map>

static unsigned int loadTextureCached(const std::string& path) {
    static std::unordered_map<std::string, unsigned int> cache;
    auto it = cache.find(path);
    if (it != cache.end()) return it->second;

    unsigned int textureID = 0;
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        cache[path] = textureID;
    }

    return textureID;
}

ChunkManager::ChunkManager(unsigned int worldSeed) : worldGen(worldSeed) {}

ChunkManager::~ChunkManager() {
    for (auto& [pos, chunk] : chunks) {
        delete chunk;
    }
    chunks.clear();
}

glm::ivec3 ChunkManager::WorldToChunk(const glm::vec3& worldPos) {
    return glm::ivec3(
        (int)std::floor(worldPos.x / CHUNK_SIZE),
        (int)std::floor(worldPos.y / CHUNK_SIZE),
        (int)std::floor(worldPos.z / CHUNK_SIZE)
    );
}

Chunk* ChunkManager::GetChunk(glm::ivec3 pos) const {
    auto it = chunks.find(pos);
    return (it != chunks.end()) ? it->second : nullptr;
}

const std::unordered_map<glm::ivec3, Chunk*, IVec3Hash, IVec3Equal>& ChunkManager::GetChunks() const {
    return chunks;
}

uint8_t ChunkManager::GetBlock(int worldX, int worldY, int worldZ) const {
    const int cx = (int)std::floor((float)worldX / CHUNK_SIZE);
    const int cy = (int)std::floor((float)worldY / CHUNK_SIZE);
    const int cz = (int)std::floor((float)worldZ / CHUNK_SIZE);

    const int lx = worldX - cx * CHUNK_SIZE;
    const int ly = worldY - cy * CHUNK_SIZE;
    const int lz = worldZ - cz * CHUNK_SIZE;

    const Chunk* chunk = GetChunk(glm::ivec3(cx, cy, cz));
    if (!chunk) return BLOCK_AIR;
    return chunk->GetBlock(lx, ly, lz);
}

void ChunkManager::SetBlock(int worldX, int worldY, int worldZ, uint8_t type) {
    const int cx = (int)std::floor((float)worldX / CHUNK_SIZE);
    const int cy = (int)std::floor((float)worldY / CHUNK_SIZE);
    const int cz = (int)std::floor((float)worldZ / CHUNK_SIZE);

    const int lx = worldX - cx * CHUNK_SIZE;
    const int ly = worldY - cy * CHUNK_SIZE;
    const int lz = worldZ - cz * CHUNK_SIZE;

    Chunk* chunk = LoadChunk(glm::ivec3(cx, cy, cz));
    if (!chunk) return;
    chunk->SetBlock(lx, ly, lz, type);
    chunk->isDirty = true;
}

Chunk* ChunkManager::LoadChunk(glm::ivec3 chunkPos) {
    if (chunks.count(chunkPos)) return chunks[chunkPos];

    Chunk* chunk = new Chunk(chunkPos);
    chunks[chunkPos] = chunk;
    worldGen.GenerateChunk(*chunk);
    return chunk;
}

void ChunkManager::UnloadChunk(glm::ivec3 chunkPos) {
    auto it = chunks.find(chunkPos);
    if (it != chunks.end()) {
        delete it->second;
        chunks.erase(it);
    }
}

void ChunkManager::Update(const glm::vec3& playerPos) {
    glm::ivec3 playerChunk = WorldToChunk(playerPos);

    // 1. Load chunks within render distance
    for (int dx = -RENDER_DISTANCE; dx <= RENDER_DISTANCE; dx++) {
        for (int dz = -RENDER_DISTANCE; dz <= RENDER_DISTANCE; dz++) {
            if (dx * dx + dz * dz > RENDER_DISTANCE * RENDER_DISTANCE) continue;

            for (int dy = -2; dy <= 4; dy++) {
                glm::ivec3 cp = playerChunk + glm::ivec3(dx, dy, dz);
                if (!chunks.count(cp)) {
                    LoadChunk(cp);
                }
            }
        }
    }

    // 2. Unload chunks that are too far
    std::vector<glm::ivec3> toUnload;
    for (auto& [pos, chunk] : chunks) {
        glm::ivec3 delta = pos - playerChunk;
        int flatDist = delta.x * delta.x + delta.z * delta.z;
        if (flatDist > (RENDER_DISTANCE + 2) * (RENDER_DISTANCE + 2)
            || delta.y < -3 || delta.y > 5) {
            toUnload.push_back(pos);
        }
    }
    for (auto& pos : toUnload) {
        UnloadChunk(pos);
    }

    // 3. Rebuild dirty meshes
    RebuildDirtyChunks();
}

void ChunkManager::RebuildDirtyChunks() {
    int rebuildsThisFrame = 0;
    const int MAX_REBUILDS = 4;

    for (auto& [pos, chunk] : chunks) {
        if (!chunk->isDirty) continue;
        if (rebuildsThisFrame >= MAX_REBUILDS) break;

        Chunk* nx = GetChunk(pos + glm::ivec3(-1,  0,  0));
        Chunk* px = GetChunk(pos + glm::ivec3( 1,  0,  0));
        Chunk* ny = GetChunk(pos + glm::ivec3( 0, -1,  0));
        Chunk* py = GetChunk(pos + glm::ivec3( 0,  1,  0));
        Chunk* nz = GetChunk(pos + glm::ivec3( 0,  0, -1));
        Chunk* pz = GetChunk(pos + glm::ivec3( 0,  0,  1));

        chunk->BuildMesh(nx, px, ny, py, nz, pz);
        rebuildsThisFrame++;
    }
}

void ChunkManager::Render(Shader& shader, const Camera& camera, int biomeType) {
    shader.use();

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 500.0f);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    shader.setInt("biomeType", biomeType);
    shader.setInt("texture1", 0);
    shader.setInt("biomeTexture", 1);
    shader.setVec3("lightPos", glm::vec3(0.0f, 60.0f, 0.0f));
    shader.setVec3("lightColor", glm::vec3(1.0f));
    shader.setVec3("objectColor", glm::vec3(1.0f));

    for (const auto& [pos, chunk] : chunks) {
        if (chunk && chunk->vertexCount > 0) {
            Biome biome = chunk->getBiome();
            unsigned int texture = biome.texturePath.empty() ? 0 : loadTextureCached(biome.texturePath);
            if (texture != 0) {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texture);
            }
            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos * CHUNK_SIZE));
            shader.setMat4("model", model);
            chunk->Draw(shader, biomeType);
        }
    }
}
