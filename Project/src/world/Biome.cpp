#include "Biome.h"
#include <stdexcept>

void BiomeManager::registerBiome(const BiomeType& type, const Biome& biome) {
    biomes[type] = biome;
}

Biome BiomeManager::getBiome(BiomeType type) const {
    auto it = biomes.find(type);
    if (it == biomes.end()) {
        throw std::runtime_error("Biome not registered: " + std::to_string(static_cast<int>(type)));
    }
    return it->second;
}

// Initialize default biomes
static void initializeDefaultBiomes() {
    BiomeManager& biomeManager = BiomeManager::getInstance();
    biomeManager.registerBiome(BiomeType::FOREST, Biome(BiomeType::FOREST, "textures/forest.png", 0.5f, 0.0f));
    biomeManager.registerBiome(BiomeType::DESERT, Biome(BiomeType::DESERT, "textures/desert.png", 1.0f, -0.5f));
    biomeManager.registerBiome(BiomeType::MOUNTAIN, Biome(BiomeType::MOUNTAIN, "textures/mountain.png", 1.5f, 1.0f));
    biomeManager.registerBiome(BiomeType::OCEAN, Biome(BiomeType::OCEAN, "textures/ocean.png", 0.3f, -1.0f));
}

// Call to initialize default biomes
static bool initialized = false;

void initializeBiomes() {
    if (!initialized) {
        initializeDefaultBiomes();
        initialized = true;
    }
}