#include "world/WorldRenderer.h"
#include "world/ChunkManager.h"
#include "world/Biome.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include <unordered_map>

// Local include for stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Cached texture loading - prevents reloading textures every frame
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
    } else {
        std::cout << "Failed to load texture at path: " << path << std::endl;
    }

    return textureID;
}

void WorldRenderer::RenderChunks(Shader& shader, ChunkManager& chunkManager, Camera& camera) {
    const auto& chunks = chunkManager.GetChunks();
    for (const auto& chunkPair : chunks) {
        if (chunkPair.second && chunkPair.second->vertexCount > 0) {
            RenderChunkWithBiome(shader, *chunkPair.second, camera, "");
        }
    }
}

void WorldRenderer::RenderChunkWithBiome(Shader& shader, Chunk& chunk, Camera& camera, std::string biomeType) {
    Biome biome = chunk.getBiome();
    int biomeTypeId = biome.getType(); // Assuming Biome has a getType() method
    // chunkPos is in chunk coordinates; convert to block coordinates.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(chunk.chunkPos * CHUNK_SIZE));

    // Set biome-specific uniforms
    shader.use();
    shader.setMat4("model", model);
    shader.setMat4("view", camera.GetViewMatrix());
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 500.0f);
    shader.setMat4("projection", projection);
    shader.setInt("texture1", 0);
    shader.setInt("biomeType", biomeTypeId);

    // Draw chunk with biome-specific texture (using cached texture loading)
    if (chunk.vertexCount > 0) {
        unsigned int biomeTexture = loadTextureCached(biome.texturePath);
        if (biomeTexture) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, biomeTexture);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, biomeTexture);
            chunk.Draw(shader, biomeTypeId);
        }
    }
}

void WorldRenderer::Render(Shader& shader, Mesh& mesh, Camera& camera, glm::vec3* positions, int count) {
    shader.use();

    glm::mat4 view       = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1280.0f / 720.0f, 0.1f, 100.0f);

    shader.setMat4("view",       view);
    shader.setMat4("projection", projection);

    for (int i = 0; i < count; i++) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), positions[i]);
        shader.setMat4("model", model);

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mesh.Draw();
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Unused method for backward compatibility
}
