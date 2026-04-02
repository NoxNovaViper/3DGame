#include "Quest.h"
#include "../entities/Player.h"
#include <iostream>
#include <algorithm>

// ==================== QuestManager Implementation ====================

QuestManager::QuestManager() {}

QuestManager::~QuestManager() {
    ClearAllQuests();
}

void QuestManager::AddQuest(const Quest& quest) {
    // Check if quest with same ID already exists
    for (const auto& existingQuest : quests) {
        if (existingQuest.id == quest.id) {
            return; // Don't add duplicate quests
        }
    }
    
    quests.push_back(quest);
}

void QuestManager::RemoveQuest(int questId) {
    quests.erase(
        std::remove_if(quests.begin(), quests.end(),
            [questId](const Quest& q) { return q.id == questId; }),
        quests.end()
    );
}

void QuestManager::ClearAllQuests() {
    quests.clear();
}

void QuestManager::Update(float deltaTime) {
    for (auto& quest : quests) {
        quest.Update();
    }
}

void QuestManager::Update() {
    for (auto& quest : quests) {
        quest.Update();
    }
}

void QuestManager::StartQuest(int questId) {
    for (auto& quest : quests) {
        if (quest.id == questId) {
            quest.Start();
            break;
        }
    }
}

void QuestManager::CompleteQuest(int questId) {
    for (auto& quest : quests) {
        if (quest.id == questId) {
            quest.Complete();
            break;
        }
    }
}

void QuestManager::FailQuest(int questId) {
    for (auto& quest : quests) {
        if (quest.id == questId) {
            quest.Fail();
            break;
        }
    }
}

bool QuestManager::IsQuestActive(int questId) const {
    for (const auto& quest : quests) {
        if (quest.id == questId && quest.state == QuestState::IN_PROGRESS) {
            return true;
        }
    }
    return false;
}

bool QuestManager::IsQuestCompleted(int questId) const {
    for (const auto& quest : quests) {
        if (quest.id == questId && quest.state == QuestState::COMPLETED) {
            return true;
        }
    }
    return false;
}

Quest* QuestManager::GetQuest(int questId) {
    for (auto& quest : quests) {
        if (quest.id == questId) {
            return &quest;
        }
    }
    return nullptr;
}

const Quest* QuestManager::GetQuest(int questId) const {
    for (const auto& quest : quests) {
        if (quest.id == questId) {
            return &quest;
        }
    }
    return nullptr;
}

std::vector<Quest*> QuestManager::GetActiveQuests() {
    std::vector<Quest*> active;
    for (auto& quest : quests) {
        if (quest.state == QuestState::IN_PROGRESS) {
            active.push_back(&quest);
        }
    }
    return active;
}

std::vector<const Quest*> QuestManager::GetCompletedQuests() const {
    std::vector<const Quest*> completed;
    for (const auto& quest : quests) {
        if (quest.state == QuestState::COMPLETED) {
            completed.push_back(&quest);
        }
    }
    return completed;
}

std::vector<const Quest*> QuestManager::GetInProgressQuests() const {
    std::vector<const Quest*> inProgress;
    for (const auto& quest : quests) {
        if (quest.state == QuestState::IN_PROGRESS) {
            inProgress.push_back(&quest);
        }
    }
    return inProgress;
}

std::vector<const Quest*> QuestManager::GetAvailableQuests(int playerLevel, const std::vector<int>& completedQuests) const {
    std::vector<const Quest*> available;
    for (const auto& quest : quests) {
        if (quest.state == QuestState::NOT_STARTED && quest.CanStart(playerLevel, completedQuests)) {
            available.push_back(&quest);
        }
    }
    return available;
}

void QuestManager::AddProgressToQuest(int questId, const std::string& targetType, int amount) {
    for (auto& quest : quests) {
        if (quest.id == questId) {
            quest.AddProgress(targetType, amount);
            break;
        }
    }
}

void QuestManager::CollectQuestRewards(int questId, Player& player) {
    for (auto& quest : quests) {
        if (quest.id == questId && quest.state == QuestState::COMPLETED) {
            quest.CollectRewards(player);
            break;
        }
    }
}

void QuestManager::TrackLocation(const glm::vec3& playerPosition) {
    // Implement location tracking for quests
    for (auto& quest : quests) {
        if (quest.type == QuestType::EXPLORE) {
            for (auto& objective : quest.objectives) {
                if (!objective.isCompleted) {
                    float distance = glm::distance(playerPosition, objective.targetLocation);
                    if (distance <= objective.targetRadius) {
                        objective.isCompleted = true;
                        quest.CheckCompletion();
                    }
                }
            }
        }
    }
}

void QuestManager::TrackKillProgress(const std::string& monsterType, int amount) {
    // Implement kill tracking for quests
    for (auto& quest : quests) {
        if (quest.type == QuestType::KILL) {
            for (auto& objective : quest.objectives) {
                if (!objective.isCompleted && objective.targetType == monsterType) {
                    objective.currentProgress += amount;
                    if (objective.currentProgress >= objective.requiredProgress) {
                        objective.isCompleted = true;
                        quest.CheckCompletion();
                    }
                }
            }
        }
    }
}

void QuestManager::TrackCollection(const std::string& itemName, int amount) {
    // Implement collection tracking for quests
    for (auto& quest : quests) {
        if (quest.type == QuestType::COLLECT) {
            for (auto& objective : quest.objectives) {
                if (!objective.isCompleted && objective.itemName == itemName) {
                    objective.currentProgress += amount;
                    if (objective.currentProgress >= objective.requiredProgress) {
                        objective.isCompleted = true;
                        quest.CheckCompletion();
                    }
                }
            }
        }
    }
}