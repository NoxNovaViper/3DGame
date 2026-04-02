#include "FloorManager.h"
#include <iostream>

FloorManager::FloorManager() {}

FloorManager::~FloorManager() {}

void FloorManager::Initialize(int totalFloors) {
    this->totalFloors = totalFloors;
    LoadFloor(1);
}

void FloorManager::Update(const glm::vec3& playerPos) {
    if (activeFloor) {
        activeFloor->Update(playerPos);
    }
}

void FloorManager::Render(Shader& shader, const Camera& camera) {
    if (activeFloor) {
        activeFloor->Render(shader, camera);
    }
}

void FloorManager::ChangeFloor(int floorNumber) {
    if (floorNumber < 1 || floorNumber > totalFloors) {
        std::cout << "Invalid floor number: " << floorNumber << std::endl;
        return;
    }
    
    std::cout << "Changing to Floor " << floorNumber << std::endl;
    currentFloor = floorNumber;
    LoadFloor(floorNumber);
}

int FloorManager::GetCurrentFloor() const {
    return currentFloor;
}

int FloorManager::GetTotalFloors() const {
    return totalFloors;
}

Floor* FloorManager::GetCurrentFloorPtr() {
    return activeFloor.get();
}

const Floor* FloorManager::GetCurrentFloorPtr() const {
    return activeFloor.get();
}

glm::vec3 FloorManager::GetFloorSpawnPoint(int floorNumber) const {
    // Return a default spawn point based on floor
    return glm::vec3(0.0f, 15.0f + floorNumber * 2.0f, 0.0f);
}

bool FloorManager::CheckFloorTransition(const glm::vec3& playerPos) {
    // Check if player is at a teleport crystal or staircase
    // For now, this is handled manually via key press
    return false;
}

void FloorManager::LoadFloor(int floorNumber) {
    activeFloor = std::make_unique<Floor>(floorNumber, floorNumber * 1000);
    std::cout << "Loaded " << activeFloor->config.name << std::endl;
}
