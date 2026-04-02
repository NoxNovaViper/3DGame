#pragma once

// Forward declarations
class Shader;
class Camera;

#include "world/Floor.h"
#include <vector>
#include <memory>

class FloorManager {
public:
    FloorManager();
    ~FloorManager();

    void Initialize(int totalFloors = 100);

    void Update(const glm::vec3& playerPos);

    void Render(Shader& shader, const Camera& camera);

    // Floor management
    void ChangeFloor(int floorNumber);
    int GetCurrentFloor() const;
    int GetTotalFloors() const;

    // Get current floor
    Floor* GetCurrentFloorPtr();
    const Floor* GetCurrentFloorPtr() const;

    // Get spawn point for a floor
    glm::vec3 GetFloorSpawnPoint(int floorNumber) const;

    // Check if player needs to change floors
    bool CheckFloorTransition(const glm::vec3& playerPos);

private:
    int currentFloor = 1;
    int totalFloors = 100;
    std::unique_ptr<Floor> activeFloor;

    void LoadFloor(int floorNumber);
};
