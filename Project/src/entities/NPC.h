#pragma once
#include "entities/Entity.h"
#include <string>
#include <vector>
#include <functional>

enum class NPCType {
    QUEST_GIVER,
    MERCHANT,
    GUIDE,
    BLACKSMITH,
    HEALER,
    VILLAGER
};

enum class NPCState {
    IDLE,
    TALKING,
    ANIMATING
};

struct DialogLine {
    std::string text;
    std::string speaker;
    bool isPlayer = false;
    std::vector<std::string> choices;  // Optional choices for branching dialog
    int nextLineIndex = -1;  // -1 means end of dialog
    int requiredQuestId = -1;  // Quest ID required to see this line
};

struct DialogTree {
    std::string title;
    std::vector<DialogLine> lines;
    int startLine = 0;
};

class NPC : public Entity {
public:
    std::string name;
    NPCType type;
    NPCState state = NPCState::IDLE;
    
    // Visual
    glm::vec3 color = glm::vec3(1.0f);
    float interactionRange = 3.0f;
    
    // Dialog
    DialogTree dialogTree;
    int currentDialogLine = 0;
    bool hasDialog = false;
    
    // Quest related
    int associatedQuestId = -1;  // -1 means no quest
    bool questAvailable = false;
    bool questCompleted = false;
    
    // Merchant inventory (if merchant type)
    struct MerchantItem {
        std::string name;
        int cost;
        int quantity;
    };
    std::vector<MerchantItem> merchantInventory;
    
    NPC();
    NPC(glm::vec3 pos, NPCType type, const std::string& name);
    ~NPC();
    
    void Update(float deltaTime) override;
    
    // Interaction
    bool CanInteract(const glm::vec3& playerPos) const;
    void StartDialog();
    void AdvanceDialog();
    bool IsDialogActive() const;
    const DialogLine* GetCurrentDialogLine() const;
    
    // Quest management
    bool HasQuestAvailable() const;
    int GetAssociatedQuestId() const;
    void MarkQuestAsCompleted();
    
    // Merchant
    bool IsMerchant() const;
    const std::vector<MerchantItem>& GetMerchantInventory() const;
    
private:
    void SetupDefaultDialog();
    void SetupMerchantInventory();
};
