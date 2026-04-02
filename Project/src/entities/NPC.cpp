#include "entities/NPC.h"
#include <iostream>

NPC::NPC() : Entity() {}

NPC::NPC(glm::vec3 pos, NPCType type, const std::string& name) 
    : Entity(pos), type(type), name(name) {
    
    // Set default colors based on type
    switch (type) {
        case NPCType::QUEST_GIVER:
            color = glm::vec3(1.0f, 0.84f, 0.0f);  // Gold
            break;
        case NPCType::MERCHANT:
            color = glm::vec3(0.0f, 1.0f, 0.0f);  // Green
            break;
        case NPCType::GUIDE:
            color = glm::vec3(0.0f, 0.8f, 1.0f);  // Light blue
            break;
        case NPCType::BLACKSMITH:
            color = glm::vec3(0.5f, 0.3f, 0.1f);  // Brown
            break;
        case NPCType::HEALER:
            color = glm::vec3(1.0f, 1.0f, 1.0f);  // White
            break;
        case NPCType::VILLAGER:
            color = glm::vec3(0.7f, 0.5f, 0.3f);  // Tan
            break;
    }
    
    // Setup default dialog
    hasDialog = true;
    SetupDefaultDialog();
    
    // Setup merchant inventory if applicable
    if (type == NPCType::MERCHANT) {
        SetupMerchantInventory();
    }
}

NPC::~NPC() {}

void NPC::Update(float deltaTime) {
    Entity::Update(deltaTime);
    
    // Update state
    if (state == NPCState::ANIMATING) {
        // Handle any animations here
    }
}

bool NPC::CanInteract(const glm::vec3& playerPos) const {
    float distance = glm::distance(Position, playerPos);
    return distance <= interactionRange;
}

void NPC::StartDialog() {
    if (hasDialog) {
        state = NPCState::TALKING;
        currentDialogLine = dialogTree.startLine;
    }
}

void NPC::AdvanceDialog() {
    if (currentDialogLine >= 0 && currentDialogLine < (int)dialogTree.lines.size()) {
        const DialogLine& line = dialogTree.lines[currentDialogLine];
        currentDialogLine = line.nextLineIndex;
        
        if (currentDialogLine < 0 || currentDialogLine >= (int)dialogTree.lines.size()) {
            state = NPCState::IDLE;
        }
    } else {
        state = NPCState::IDLE;
    }
}

bool NPC::IsDialogActive() const {
    return state == NPCState::TALKING;
}

const DialogLine* NPC::GetCurrentDialogLine() const {
    if (currentDialogLine >= 0 && currentDialogLine < (int)dialogTree.lines.size()) {
        return &dialogTree.lines[currentDialogLine];
    }
    return nullptr;
}

bool NPC::HasQuestAvailable() const {
    return questAvailable && !questCompleted && associatedQuestId >= 0;
}

int NPC::GetAssociatedQuestId() const {
    return associatedQuestId;
}

void NPC::MarkQuestAsCompleted() {
    questCompleted = true;
    questAvailable = false;
}

bool NPC::IsMerchant() const {
    return type == NPCType::MERCHANT;
}

const std::vector<NPC::MerchantItem>& NPC::GetMerchantInventory() const {
    return merchantInventory;
}

void NPC::SetupDefaultDialog() {
    dialogTree.title = "Default Dialog";
    
    switch (type) {
        case NPCType::QUEST_GIVER:
            dialogTree.title = "Quest Giver";
            dialogTree.lines = {
                {"Welcome, adventurer! I have a task for you.", name,     false, {}, 1},
                {"What kind of task?",                          "Kirito", true,  {}, 2},
                {"Monsters have been gathering near the village. Defeat 5 of them and I'll reward you handsomely.", name, false, {}, 3},
                {"I'll help you with that!",                    "Kirito", true,  {}, -1}
            };
            break;

        case NPCType::MERCHANT:
            dialogTree.title = "Merchant";
            dialogTree.lines = {
                {"Welcome to my shop! Take a look at my wares.", name,     false, {}, 1},
                {"I'd like to buy something.",                   "Kirito", true,  {}, -1}
            };
            break;

        case NPCType::GUIDE:
            dialogTree.title = "Guide";
            dialogTree.lines = {
                {"Hello! I'm here to help you navigate Aincrad.",                                                    name,     false, {}, 1},
                {"Where am I?",                                                                                       "Kirito", true,  {}, 2},
                {"You're on Floor 1, the Town of Beginnings. Explore carefully and watch out for monsters!", name, false, {}, -1}
            };
            break;

        case NPCType::HEALER:
            dialogTree.title = "Healer";
            dialogTree.lines = {
                {"You look injured. Let me help you.", name,     false, {}, 1},
                {"Thank you.",                         "Kirito", true,  {}, -1}
            };
            break;
            
        default:
            dialogTree.lines = {
                {"Hello there!", name, false, {}, -1}
            };
            break;
    }
}

void NPC::SetupMerchantInventory() {
    merchantInventory = {
        {"Health Potion", 50, 10},
        {"Antidote", 30, 5},
        {"Iron Sword", 200, 3},
        {"Leather Armor", 150, 2},
        {"Teleport Crystal", 500, 1}
    };
}
