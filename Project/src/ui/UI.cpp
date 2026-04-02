#include "ui/UI.h"

// ==================== UIElement ====================

UIElement::UIElement()
    : type(UIElementType::TEXT), position(0.0f), size(0.1f), color(1.0f), fontSize(14.0f), fillAmount(1.0f) {}

UIElement::UIElement(const std::string& id, UIElementType type, glm::vec2 pos, glm::vec2 sz)
    : id(id), type(type), position(pos), size(sz), color(1.0f), fontSize(14.0f), fillAmount(1.0f) {}

// ==================== HUDPanel ====================

HUDPanel::HUDPanel()
    : position(0.0f), size(0.1f), backgroundColor(0.0f, 0.0f, 0.0f, 0.5f) {}

HUDPanel::HUDPanel(const std::string& id, glm::vec2 pos, glm::vec2 sz)
    : id(id), position(pos), size(sz), backgroundColor(0.0f, 0.0f, 0.0f, 0.5f) {}

void HUDPanel::AddElement(const UIElement& element) {
    elements.push_back(element);
}

UIElement* HUDPanel::GetElement(const std::string& elementId) {
    for (auto& el : elements) {
        if (el.id == elementId) return &el;
    }
    return nullptr;
}

// ==================== UIHUD ====================

UIHUD::UIHUD() : screenWidth(1280), screenHeight(720) {}

UIHUD::~UIHUD() {}

void UIHUD::Initialize(int w, int h) {
    screenWidth  = w;
    screenHeight = h;

    SetupPlayerPanel();
    SetupQuestPanel();
    SetupDialogPanel();
    SetupCombatPanel();
    SetupMinimapPanel();
    SetupCrosshair();
    SetupNotificationPanel();

    allPanels = {
        &playerPanel, &questPanel, &dialogPanel,
        &combatPanel, &minimapPanel, &crosshairPanel, &notificationPanel
    };
}

// ---- Update methods ----

void UIHUD::UpdatePlayerHUD(float currentHP, float maxHP,
                             float currentMP, float maxMP,
                             int level, int exp, int expToNext,
                             int col, const std::string& playerName) {
    if (UIElement* hp  = playerPanel.GetElement("hp_bar"))
        hp->fillAmount = (maxHP  > 0.0f) ? currentHP  / maxHP  : 0.0f;
    if (UIElement* mp  = playerPanel.GetElement("mp_bar"))
        mp->fillAmount = (maxMP  > 0.0f) ? currentMP  / maxMP  : 0.0f;
    if (UIElement* exp_bar = playerPanel.GetElement("exp_bar"))
        exp_bar->fillAmount = (expToNext > 0) ? (float)exp / (float)expToNext : 0.0f;
    if (UIElement* name = playerPanel.GetElement("player_name"))
        name->text = playerName + "  Lv." + std::to_string(level);
    if (UIElement* col_el = playerPanel.GetElement("col_display"))
        col_el->text = std::to_string(col) + " Col";
}

void UIHUD::UpdateQuestDisplay(const std::string& questTitle,
                                const std::string& questObjective,
                                float questProgress) {
    if (UIElement* title = questPanel.GetElement("quest_title"))
        title->text = questTitle;
    if (UIElement* obj = questPanel.GetElement("quest_objective"))
        obj->text = questObjective;
    if (UIElement* bar = questPanel.GetElement("quest_progress"))
        bar->fillAmount = questProgress;
}

void UIHUD::UpdateDialogDisplay(const std::string& speaker,
                                 const std::string& dialogText,
                                 const std::vector<std::string>& choices) {
    if (UIElement* spk = dialogPanel.GetElement("dialog_speaker"))
        spk->text = speaker;
    if (UIElement* txt = dialogPanel.GetElement("dialog_text"))
        txt->text = dialogText;
    dialogPanel.visible = !dialogText.empty();
}

void UIHUD::UpdateCombatDisplay(bool inCombat,
                                 const std::string& targetName,
                                 float targetHP, float targetMaxHP) {
    combatPanel.visible = inCombat;
    if (!inCombat) return;
    if (UIElement* name = combatPanel.GetElement("target_name"))
        name->text = targetName;
    if (UIElement* bar = combatPanel.GetElement("target_hp"))
        bar->fillAmount = (targetMaxHP > 0.0f) ? targetHP / targetMaxHP : 0.0f;
}

void UIHUD::UpdateMinimap(float playerX, float playerZ,
                           const std::vector<glm::vec2>& nearbyEntities) {
    // Minimap rendering is handled externally via GetRenderCommands();
    // Player dot position stored implicitly via panel center
    (void)playerX; (void)playerZ; (void)nearbyEntities;
}

// ---- Panel visibility ----

void UIHUD::ShowPanel(const std::string& panelId) {
    for (auto* p : allPanels) if (p->id == panelId) { p->visible = true; return; }
}

void UIHUD::HidePanel(const std::string& panelId) {
    for (auto* p : allPanels) if (p->id == panelId) { p->visible = false; return; }
}

void UIHUD::TogglePanel(const std::string& panelId) {
    for (auto* p : allPanels) if (p->id == panelId) { p->visible = !p->visible; return; }
}

bool UIHUD::IsPanelVisible(const std::string& panelId) const {
    for (const auto* p : allPanels) if (p->id == panelId) return p->visible;
    return false;
}

// ---- Render commands ----

std::vector<UIHUD::RenderCommand> UIHUD::GetRenderCommands() const {
    std::vector<RenderCommand> commands;

    for (const auto* panel : allPanels) {
        if (!panel->visible) continue;

        RenderCommand bg;
        bg.position        = panel->position;
        bg.size            = panel->size;
        bg.color           = panel->backgroundColor;
        bg.isText          = false;
        bg.isBar           = false;
        bg.fillAmount      = 1.0f;
        bg.fillColor       = panel->backgroundColor;
        bg.backgroundColor = panel->backgroundColor;
        commands.push_back(bg);

        for (const auto& el : panel->elements) {
            if (!el.visible) continue;

            RenderCommand cmd;
            cmd.position        = el.position;
            cmd.size            = el.size;
            cmd.color           = el.color;
            cmd.text            = el.text;
            cmd.fontSize        = el.fontSize;
            cmd.isText          = (el.type == UIElementType::TEXT);
            cmd.isBar           = (el.type == UIElementType::BAR);
            cmd.fillAmount      = el.fillAmount;
            cmd.fillColor       = el.fillColor;
            cmd.backgroundColor = el.backgroundColor;
            commands.push_back(cmd);
        }
    }

    return commands;
}

// ---- Input ----

bool UIHUD::HandleClick(float screenX, float screenY) {
    glm::vec2 norm = ScreenToNormalized(glm::vec2(screenX, screenY));
    for (auto* panel : allPanels) {
        if (!panel->visible) continue;
        for (auto& el : panel->elements) {
            if (!el.visible || !el.enabled || el.type != UIElementType::BUTTON) continue;
            if (norm.x >= el.position.x && norm.x <= el.position.x + el.size.x &&
                norm.y >= el.position.y && norm.y <= el.position.y + el.size.y) {
                return true;
            }
        }
    }
    return false;
}

bool UIHUD::HandleKeyPress(int key) {
    (void)key;
    return false;
}

// ---- Setup helpers ----

void UIHUD::SetupPlayerPanel() {
    playerPanel = HUDPanel("player_panel", glm::vec2(0.01f, 0.01f), glm::vec2(0.22f, 0.18f));
    playerPanel.backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.55f);

    UIElement name("player_name", UIElementType::TEXT, glm::vec2(0.02f, 0.02f), glm::vec2(0.18f, 0.03f));
    name.text = "Player  Lv.1";
    name.color = glm::vec4(1.0f);
    playerPanel.AddElement(name);

    UIElement hp("hp_bar", UIElementType::BAR, glm::vec2(0.02f, 0.06f), glm::vec2(0.18f, 0.025f));
    hp.fillColor = glm::vec4(0.8f, 0.1f, 0.1f, 1.0f);
    playerPanel.AddElement(hp);

    UIElement mp("mp_bar", UIElementType::BAR, glm::vec2(0.02f, 0.10f), glm::vec2(0.18f, 0.025f));
    mp.fillColor = glm::vec4(0.1f, 0.3f, 0.9f, 1.0f);
    playerPanel.AddElement(mp);

    UIElement exp_bar("exp_bar", UIElementType::BAR, glm::vec2(0.02f, 0.14f), glm::vec2(0.18f, 0.015f));
    exp_bar.fillColor = glm::vec4(0.9f, 0.8f, 0.1f, 1.0f);
    playerPanel.AddElement(exp_bar);

    UIElement col("col_display", UIElementType::TEXT, glm::vec2(0.02f, 0.16f), glm::vec2(0.1f, 0.02f));
    col.text = "0 Col";
    col.color = glm::vec4(0.9f, 0.8f, 0.2f, 1.0f);
    col.fontSize = 12.0f;
    playerPanel.AddElement(col);
}

void UIHUD::SetupQuestPanel() {
    questPanel = HUDPanel("quest_panel", glm::vec2(0.75f, 0.02f), glm::vec2(0.23f, 0.12f));
    questPanel.backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);

    UIElement title("quest_title", UIElementType::TEXT, glm::vec2(0.76f, 0.03f), glm::vec2(0.21f, 0.03f));
    title.text = "No active quest";
    title.color = glm::vec4(1.0f, 0.85f, 0.3f, 1.0f);
    title.fontSize = 13.0f;
    questPanel.AddElement(title);

    UIElement obj("quest_objective", UIElementType::TEXT, glm::vec2(0.76f, 0.07f), glm::vec2(0.21f, 0.025f));
    obj.text = "";
    obj.color = glm::vec4(0.9f, 0.9f, 0.9f, 1.0f);
    obj.fontSize = 11.0f;
    questPanel.AddElement(obj);

    UIElement bar("quest_progress", UIElementType::BAR, glm::vec2(0.76f, 0.11f), glm::vec2(0.21f, 0.015f));
    bar.fillColor = glm::vec4(0.3f, 0.8f, 0.3f, 1.0f);
    bar.fillAmount = 0.0f;
    questPanel.AddElement(bar);
}

void UIHUD::SetupDialogPanel() {
    dialogPanel = HUDPanel("dialog_panel", glm::vec2(0.1f, 0.7f), glm::vec2(0.8f, 0.25f));
    dialogPanel.backgroundColor = glm::vec4(0.0f, 0.05f, 0.15f, 0.88f);
    dialogPanel.visible = false;

    UIElement speaker("dialog_speaker", UIElementType::TEXT, glm::vec2(0.12f, 0.71f), glm::vec2(0.3f, 0.03f));
    speaker.color = glm::vec4(0.5f, 0.8f, 1.0f, 1.0f);
    speaker.fontSize = 14.0f;
    dialogPanel.AddElement(speaker);

    UIElement text("dialog_text", UIElementType::TEXT, glm::vec2(0.12f, 0.75f), glm::vec2(0.76f, 0.16f));
    text.color = glm::vec4(1.0f);
    text.fontSize = 13.0f;
    dialogPanel.AddElement(text);
}

void UIHUD::SetupCombatPanel() {
    combatPanel = HUDPanel("combat_panel", glm::vec2(0.3f, 0.02f), glm::vec2(0.4f, 0.06f));
    combatPanel.backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.5f);
    combatPanel.visible = false;

    UIElement name("target_name", UIElementType::TEXT, glm::vec2(0.35f, 0.025f), glm::vec2(0.3f, 0.025f));
    name.color = glm::vec4(1.0f, 0.4f, 0.4f, 1.0f);
    name.fontSize = 13.0f;
    combatPanel.AddElement(name);

    UIElement hp("target_hp", UIElementType::BAR, glm::vec2(0.31f, 0.055f), glm::vec2(0.38f, 0.02f));
    hp.fillColor = glm::vec4(0.9f, 0.15f, 0.15f, 1.0f);
    combatPanel.AddElement(hp);
}

void UIHUD::SetupMinimapPanel() {
    minimapPanel = HUDPanel("minimap_panel", glm::vec2(0.78f, 0.16f), glm::vec2(0.2f, 0.2f));
    minimapPanel.backgroundColor = glm::vec4(0.05f, 0.1f, 0.05f, 0.7f);
}

void UIHUD::SetupCrosshair() {
    crosshairPanel = HUDPanel("crosshair_panel", glm::vec2(0.495f, 0.495f), glm::vec2(0.01f, 0.01f));
    crosshairPanel.backgroundColor = glm::vec4(0.0f);

    UIElement dot("crosshair_dot", UIElementType::IMAGE, glm::vec2(0.498f, 0.498f), glm::vec2(0.004f, 0.004f));
    dot.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.8f);
    crosshairPanel.AddElement(dot);
}

void UIHUD::SetupNotificationPanel() {
    notificationPanel = HUDPanel("notification_panel", glm::vec2(0.3f, 0.85f), glm::vec2(0.4f, 0.1f));
    notificationPanel.backgroundColor = glm::vec4(0.0f);
    notificationPanel.visible = false;
}

// ---- Coordinate helpers ----

glm::vec2 UIHUD::NormalizeToScreen(glm::vec2 normalized) const {
    return glm::vec2(normalized.x * screenWidth, normalized.y * screenHeight);
}

glm::vec2 UIHUD::ScreenToNormalized(glm::vec2 screen) const {
    return glm::vec2(screen.x / screenWidth, screen.y / screenHeight);
}
