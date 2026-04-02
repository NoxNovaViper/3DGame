#include "quest/Quest.h"
#include <iostream>
#include <sstream>
#include <algorithm>

#include "entities/Player.h"

// ==================== Quest Implementation ====================

Quest::Quest() : id(0), type(QuestType::KILL) {}

Quest::Quest(int id, const std::string& title, const std::string& description, QuestType type)
    : id(id), title(title), description(description), type(type) {
    SetupDefaultObjectives();
}

Quest::~Quest() {}

void Quest::Start() {
    state = QuestState::IN_PROGRESS;
    for (auto& obj : objectives) {
        obj.currentProgress = 0;
        obj.isCompleted = false;
    }
}

void Quest::Update() {
    if (state == QuestState::IN_PROGRESS) {
        CheckCompletion();
    }
}

void Quest::Complete() {
    state = QuestState::COMPLETED;
}

void Quest::Fail() {
    state = QuestState::FAILED;
}

void Quest::AddProgress(const std::string& targetType, int amount) {
    if (state != QuestState::IN_PROGRESS) return;
    
    for (auto& obj : objectives) {
        if (!obj.isCompleted && obj.targetType == targetType) {
            obj.currentProgress += amount;
            if (obj.currentProgress >= obj.requiredProgress) {
                obj.currentProgress = obj.requiredProgress;
                obj.isCompleted = true;
            }
        }
    }
    CheckCompletion();
}

void Quest::CheckCompletion() {
    if (state != QuestState::IN_PROGRESS) return;
    
    bool allCompleted = true;
    for (const auto& obj : objectives) {
        if (!obj.isCompleted) {
            allCompleted = false;
            break;
        }
    }
    
    if (allCompleted && !objectives.empty()) {
        Complete();
    }
}

bool Quest::IsCompleted() const {
    return state == QuestState::COMPLETED;
}

bool Quest::CanStart(int playerLevel, const std::vector<int>& completedQuests) const {
    if (state != QuestState::NOT_STARTED) return false;
    if (playerLevel < requiredLevel) return false;
    if (prerequisiteQuestId >= 0) {
        bool hasPrereq = false;
        for (int qid : completedQuests) {
            if (qid == prerequisiteQuestId) {
                hasPrereq = true;
                break;
            }
        }
        if (!hasPrereq) return false;
    }
    return true;
}

float Quest::GetProgressPercentage() const {
    if (objectives.empty()) return 0.0f;
    
    float totalProgress = 0.0f;
    for (const auto& obj : objectives) {
        totalProgress += (float)obj.currentProgress / (float)obj.requiredProgress;
    }
    return totalProgress / (float)objectives.size();
}

std::string Quest::GetSummary() const {
    return title + " - " + description;
}

std::string Quest::GetObjectiveStatus() const {
    std::stringstream ss;
    for (const auto& obj : objectives) {
        ss << obj.description << ": " << obj.currentProgress << "/" << obj.requiredProgress;
        if (obj.isCompleted) ss << " [DONE]";
        ss << "\n";
    }
    return ss.str();
}

void Quest::CollectRewards(Player& player) {
    if (state != QuestState::COMPLETED) return;
    
    player.GainEXP((float)expReward);
    player.stats.col += colReward;
    
    // In a real game, item rewards would be added to the player's inventory
    std::cout << "Quest '" << title << "' rewards collected: " 
              << expReward << " EXP, " << colReward << " Col." << std::endl;
}

void Quest::SetupDefaultObjectives() {
    switch (type) {
        case QuestType::KILL:
            objectives.push_back({
                "Defeat monsters in the area",
                0, 5, false, "monster"
            });
            break;
        case QuestType::COLLECT:
            objectives.push_back({
                "Collect items",
                0, 3, false, "", "item"
            });
            break;
        case QuestType::EXPLORE:
            objectives.push_back({
                "Explore the designated area",
                0, 1, false
            });
            break;
        default:
            break;
    }
}

#if 0
// ==================== QuestManager Implementation (duplicate) ====================

QuestManager::QuestManager() {}

QuestManager::~QuestManager() {}

void QuestManager::AddQuest(const Quest& quest) {
    quests.push_back(quest);
}

void QuestManager::RemoveQuest(int questId) {
    quests.erase(
        std::remove_if(quests.begin(), quests.end(),
            [questId](const Quest& q) { return q.id == questId; }),
        quests.end()
    );
}

Quest* QuestManager::GetQuest(int questId) {
    for (auto& q : quests) {
        if (q.id == questId) return &q;
    }
    return nullptr;
}

const Quest* QuestManager::GetQuest(int questId) const {
    for (const auto& q : quests) {
        if (q.id == questId) return &q;
    }
    return nullptr;
}

std::vector<Quest*> QuestManager::GetActiveQuests() {
    std::vector<Quest*> active;
    for (auto& q : quests) {
        if (q.state == QuestState::IN_PROGRESS) {
            active.push_back(&q);
        }
    }
    return active;
}

std::vector<const Quest*> QuestManager::GetCompletedQuests() const {
    std::vector<const Quest*> completed;
    for (const auto& q : quests) {
        if (q.state == QuestState::COMPLETED) {
            completed.push_back(&q);
        }
    }
    return completed;
}

std::vector<const Quest*> QuestManager::GetAvailableQuests(int playerLevel, const std::vector<int>& completedQuests) const {
    std::vector<const Quest*> available;
    for (const auto& q : quests) {
        if (q.state == QuestState::NOT_STARTED && q.CanStart(playerLevel, completedQuests)) {
            available.push_back(&q);
        }
    }
    return available;
}

void QuestManager::Update() {
    for (auto& q : quests) {
        if (q.state == QuestState::IN_PROGRESS) {
            q.Update();
        }
    }
}

void QuestManager::TrackKillProgress(const std::string& monsterType, int amount) {
    for (auto& q : quests) {
        if (q.state == QuestState::IN_PROGRESS) {
            q.AddProgress(monsterType, amount);
        }
    }
}

void QuestManager::TrackCollection(const std::string& itemName, int amount) {
    for (auto& q : quests) {
        if (q.state == QuestState::IN_PROGRESS) {
            q.AddProgress(itemName, amount);
        }
    }
}

void QuestManager::TrackLocation(const glm::vec3& playerPos) {
    for (auto& q : quests) {
        if (q.state != QuestState::IN_PROGRESS) continue;
        
        for (auto& obj : q.objectives) {
            if (obj.isCompleted) continue;
            
            float distance = glm::distance(playerPos, obj.targetLocation);
            if (distance <= obj.targetRadius) {
                obj.currentProgress = obj.requiredProgress;
                obj.isCompleted = true;
            }
        }
        q.CheckCompletion();
    }
}

std::vector<const Quest*> QuestManager::GetInProgressQuests() const {
    std::vector<const Quest*> inProgress;
    for (const auto& q : quests) {
        if (q.state == QuestState::IN_PROGRESS) {
            inProgress.push_back(&q);
        }
    }
    return inProgress;
}

void QuestManager::ClearAllQuests() {
    quests.clear();
}

void QuestManager::StartQuest(int questId) {
    for (auto& q : quests) {
        if (q.id == questId) { q.Start(); break; }
    }
}

void QuestManager::CompleteQuest(int questId) {
    for (auto& q : quests) {
        if (q.id == questId) { q.Complete(); break; }
    }
}

void QuestManager::FailQuest(int questId) {
    for (auto& q : quests) {
        if (q.id == questId) { q.Fail(); break; }
    }
}

bool QuestManager::IsQuestActive(int questId) const {
    for (const auto& q : quests) {
        if (q.id == questId) return q.state == QuestState::IN_PROGRESS;
    }
    return false;
}

bool QuestManager::IsQuestCompleted(int questId) const {
    for (const auto& q : quests) {
        if (q.id == questId) return q.state == QuestState::COMPLETED;
    }
    return false;
}

void QuestManager::AddProgressToQuest(int questId, const std::string& targetType, int amount) {
    for (auto& q : quests) {
        if (q.id == questId) { q.AddProgress(targetType, amount); break; }
    }
}

void QuestManager::CollectQuestRewards(int questId, Player& player) {
    for (auto& q : quests) {
        if (q.id == questId && q.state == QuestState::COMPLETED) {
            q.CollectRewards(player);
            break;
        }
    }
}

void QuestManager::Update(float deltaTime) {
    // deltaTime unused at this layer — quest logic is tick-based
    Update();
}
#endif
