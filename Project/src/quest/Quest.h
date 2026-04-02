#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

// Forward declaration
class Player;

enum class QuestType {
    KILL,           // Kill X monsters
    COLLECT,        // Collect X items
    EXPLORE,        // Reach a location
    DELIVER,        // Deliver an item
    ESCORT          // Escort an NPC
};

enum class QuestState {
    NOT_STARTED,
    IN_PROGRESS,
    COMPLETED,
    FAILED
};

struct QuestObjective {
    std::string description;
    int currentProgress = 0;
    int requiredProgress = 1;
    bool isCompleted = false;
    
    // For kill quests
    std::string targetType;
    
    // For collect quests
    std::string itemName;
    
    // For explore quests
    glm::vec3 targetLocation;
    float targetRadius = 5.0f;
};

class Quest {
public:
    int id;
    std::string title;
    std::string description;
    QuestType type;
    QuestState state = QuestState::NOT_STARTED;
    
    // Objectives
    std::vector<QuestObjective> objectives;
    
    // Rewards
    int expReward = 0;
    int colReward = 0;  // Currency
    std::vector<std::string> itemRewards;
    
    // Requirements
    int requiredLevel = 1;
    int prerequisiteQuestId = -1;  // -1 means no prerequisite
    
    // NPC association
    int questGiverId = -1;
    
    Quest();
    Quest(int id, const std::string& title, const std::string& description, QuestType type);
    ~Quest();
    
    // Quest progression
    void Start();
    void Update();
    void Complete();
    void Fail();
    
    // Progress tracking
    void AddProgress(const std::string& targetType, int amount = 1);
    void CheckCompletion();
    bool IsCompleted() const;
    bool CanStart(int playerLevel, const std::vector<int>& completedQuests) const;
    
    // Get progress as percentage (0.0 to 1.0)
    float GetProgressPercentage() const;
    
    // Get summary string
    std::string GetSummary() const;
    std::string GetObjectiveStatus() const;
    
    // Reward collection
    void CollectRewards(class Player& player);
    
private:
    void SetupDefaultObjectives();
};

// Quest Manager - handles all active quests
class QuestManager {
public:
    QuestManager();
    ~QuestManager();
    
    // Quest management
    void AddQuest(const Quest& quest);
    void RemoveQuest(int questId);
    void ClearAllQuests();
    Quest* GetQuest(int questId);
    const Quest* GetQuest(int questId) const;
    
    // Get quests by state
    std::vector<Quest*> GetActiveQuests();
    std::vector<const Quest*> GetCompletedQuests() const;
    std::vector<const Quest*> GetInProgressQuests() const;
    std::vector<const Quest*> GetAvailableQuests(int playerLevel, const std::vector<int>& completedQuests) const;
    
   // Update all active quests
   void Update(float deltaTime);
   void Update();
    
    // Quest state management
    void StartQuest(int questId);
    void CompleteQuest(int questId);
    void FailQuest(int questId);
    bool IsQuestActive(int questId) const;
    bool IsQuestCompleted(int questId) const;
    
   // Track progress for all relevant quests
   void AddProgressToQuest(int questId, const std::string& targetType, int amount = 1);
   void CollectQuestRewards(int questId, Player& player);
   
   // Location tracking
   void TrackLocation(const glm::vec3& playerPosition);
   
   // Kill tracking
   void TrackKillProgress(const std::string& monsterType, int amount = 1);
   
   // Collection tracking
   void TrackCollection(const std::string& itemName, int amount = 1);
    
private:
    std::vector<Quest> quests;
};
