#pragma once
#include "core/Camera.h"
#include "world/ChunkManager.h"
#include "world/Chunk.h"
#include "core/Shader.h"
#include "core/Mesh.h"
#include <glm/glm.hpp>
#include <string>

class WorldRenderer {
    public:
        void RenderChunks(Shader& shader, ChunkManager& chunkManager, Camera& camera);
        void RenderChunkWithBiome(Shader& shader, Chunk& chunk, Camera& camera, std::string biomeType);
        void Render(Shader& shader, Mesh& mesh, Camera& camera, glm::vec3 positions[], int count);

    private:
        std::string currentBiomeTexture;
};