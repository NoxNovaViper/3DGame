#pragma once
#include <map>
#include <string>
#include "core/Shader.h"

class ResourceManager {
public:
    // This tells the compiler the map exists, but doesn't create it yet
    static std::map<std::string, Shader*> Shaders;

    // These are just "signatures" (promises)
    static void LoadShader(const std::string& name, const char* vSource, const char* fSource);
    static Shader* GetShader(const std::string& name);
    static void Clear();
};