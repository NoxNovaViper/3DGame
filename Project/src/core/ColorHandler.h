#pragma once
#include <glm/glm.hpp>
#include <map>
#include <string>

class ColorHandler {
public:
    // A map to store colors by name
    static std::map<std::string, glm::vec3> Palette;

    // Initialize the default colors
    static void Init();

    // Helper to get a color easily
    static glm::vec3 Get(const std::string& name);
};