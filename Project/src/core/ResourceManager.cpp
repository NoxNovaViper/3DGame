#include "core/ResourceManager.h"

// 1. Actually create the memory for the map
std::map<std::string, Shader*> ResourceManager::Shaders;

// 2. The logic for loading
void ResourceManager::LoadShader(const std::string& name, const char* vSource, const char* fSource) {
    if (Shaders.find(name) != Shaders.end()) {
        delete Shaders[name];
    }
    Shaders[name] = new Shader(vSource, fSource);
}

// 3. The logic for getting
Shader* ResourceManager::GetShader(const std::string& name) {
    if (Shaders.find(name) != Shaders.end()) {
        return Shaders[name];
    }
    return nullptr; 
}

// 4. The logic for cleaning up
void ResourceManager::Clear() {
    for (auto const& [name, shader] : Shaders) {
        delete shader;
    }
    Shaders.clear();
}