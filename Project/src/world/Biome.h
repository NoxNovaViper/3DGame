#pragma once

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

enum class BiomeType {
    FOREST,
    DESERT,
    MOUNTAIN,
    OCEAN
};

class Biome {
public:
    BiomeType type;
    std::string texturePath;
    float noiseScale;
    float elevationOffset;

    int getType() const { return static_cast<int>(type); }

    Biome(BiomeType type, std::string texturePath, float noiseScale = 1.0f, float elevationOffset = 0.0f)
        : type(type), texturePath(std::move(texturePath)), noiseScale(noiseScale), elevationOffset(elevationOffset) {}
};

class BiomeManager {
public:
    static BiomeManager& getInstance() {
        static BiomeManager instance;
        return instance;
    }
    
    void registerBiome(const BiomeType& type, const Biome& biome);
    Biome getBiome(BiomeType type) const;
    
private:
    BiomeManager() = default;
    std::unordered_map<BiomeType, Biome> biomes;
};