#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>

// UI Element types
enum class UIElementType {
    TEXT,
    BAR,
    BUTTON,
    IMAGE,
    ICON
};

// Base UI Element
struct UIElement {
    std::string id;
    UIElementType type;
    glm::vec2 position;  // Screen coordinates (0-1 normalized)
    glm::vec2 size;      // Normalized size (0-1)
    glm::vec4 color;     // RGBA
    bool visible = true;
    bool enabled = true;
    
    // Text properties
    std::string text;
    float fontSize = 14.0f;
    
    // Bar properties (for health bars, etc.)
    float fillAmount = 1.0f;  // 0.0 to 1.0
    glm::vec4 fillColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 0.8f);
    
    UIElement();
    UIElement(const std::string& id, UIElementType type, glm::vec2 pos, glm::vec2 size);
};

// HUD Panel
struct HUDPanel {
    std::string id;
    glm::vec2 position;
    glm::vec2 size;
    glm::vec4 backgroundColor;
    bool visible = true;
    std::vector<UIElement> elements;
    
    HUDPanel();
    HUDPanel(const std::string& id, glm::vec2 pos, glm::vec2 size);
    
    void AddElement(const UIElement& element);
    UIElement* GetElement(const std::string& elementId);
};

// Main HUD class
class UIHUD {
public:
    UIHUD();
    ~UIHUD();
    
    // Initialize default HUD layout
    void Initialize(int screenWidth, int screenHeight);
    
    // Update HUD with player data
    void UpdatePlayerHUD(float currentHP, float maxHP, 
                         float currentMP, float maxMP,
                         int level, int exp, int expToNext,
                         int col, const std::string& playerName);
    
    // Update quest display
    void UpdateQuestDisplay(const std::string& questTitle, 
                           const std::string& questObjective,
                           float questProgress);
    
    // Update dialog display
    void UpdateDialogDisplay(const std::string& speaker, 
                            const std::string& dialogText,
                            const std::vector<std::string>& choices);
    
    // Update combat display
    void UpdateCombatDisplay(bool inCombat, 
                            const std::string& targetName,
                            float targetHP, float targetMaxHP);
    
    // Update minimap
    void UpdateMinimap(float playerX, float playerZ, 
                      const std::vector<glm::vec2>& nearbyEntities);
    
    // Show/hide panels
    void ShowPanel(const std::string& panelId);
    void HidePanel(const std::string& panelId);
    void TogglePanel(const std::string& panelId);
    
    // Get panel visibility
    bool IsPanelVisible(const std::string& panelId) const;
    
    // Render all visible elements (returns list of render commands)
    struct RenderCommand {
        glm::vec2 position;
        glm::vec2 size;
        glm::vec4 color;
        std::string text;
        float fontSize;
        bool isText;
        bool isBar;
        float fillAmount;
        glm::vec4 fillColor;
        glm::vec4 backgroundColor;
    };
    
    std::vector<RenderCommand> GetRenderCommands() const;
    
    // Handle input
    bool HandleClick(float screenX, float screenY);
    bool HandleKeyPress(int key);
    
private:
    int screenWidth;
    int screenHeight;
    
    // Panels
    HUDPanel playerPanel;      // HP, MP, Level, Name
    HUDPanel questPanel;       // Active quest info
    HUDPanel dialogPanel;      // NPC dialog
    HUDPanel combatPanel;      // Combat info
    HUDPanel minimapPanel;     // Minimap
    HUDPanel crosshairPanel;   // Center crosshair
    HUDPanel notificationPanel; // Temporary notifications
    
    std::vector<HUDPanel*> allPanels;
    
    // Notifications
    struct Notification {
        std::string text;
        float duration;
        float timer;
    };
    std::vector<Notification> notifications;
    
    void SetupPlayerPanel();
    void SetupQuestPanel();
    void SetupDialogPanel();
    void SetupCombatPanel();
    void SetupMinimapPanel();
    void SetupCrosshair();
    void SetupNotificationPanel();
    
    // Convert normalized coords to screen coords
    glm::vec2 NormalizeToScreen(glm::vec2 normalized) const;
    glm::vec2 ScreenToNormalized(glm::vec2 screen) const;
};
