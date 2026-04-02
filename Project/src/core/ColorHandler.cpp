#include "core/ColorHandler.h"

// Define the static map
std::map<std::string, glm::vec3> ColorHandler::Palette;

void ColorHandler::Init() {
    // 1. Grass color (normalized to [0,1])
    Palette["grass"] = glm::vec3(0.3f, 0.7f, 0.15f);

    // 2. Sky color (normalized to [0,1])
    Palette["sky"] = glm::vec3(0.1f, 0.6f, 0.9f);

    // 3. Grass outline (dark green) - FIX: was missing, caused white background
    Palette["grass_outline"] = glm::vec3(0.05f, 0.15f, 0.05f);

    // 4. Dirt/Stone for future use
    Palette["dirt"] = glm::vec3(0.4f, 0.2f, 0.1f);
    Palette["stone"] = glm::vec3(0.5f, 0.5f, 0.5f);
}

glm::vec3 ColorHandler::Get(const std::string& name) {
    if (Palette.find(name) != Palette.end()) {
        return Palette[name];
    }
    return glm::vec3(1.0f); // Default to White if not found
}