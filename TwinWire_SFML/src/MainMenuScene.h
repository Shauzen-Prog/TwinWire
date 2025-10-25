#pragma once
#include "IScene.h"
#include "ResouceManager.h"
#include "UIManager.h"
#include "AudioOptionsPanel.h"

class MainMenuScene : public IScene
{
public:
    MainMenuScene();
    ~MainMenuScene() override = default;
    
    void onEnter(Game& game) override;
    void onExit(Game& game) override;
    void handleEvent(Game& game, const sf::Event& ev) override;
    void update(Game& game, float dt) override;
    void draw(Game& game, sf::RenderTarget& rt) override;

private:
    ResouceManager m_rm;
    
    ResouceManager::FontPtr m_font;
    sf::Text m_title;
    sf::FloatRect m_cullRect{};
    sf::Text m_best; // <- "Mejor: AJX 3:42 M:2"

    UIManager m_ui;
    
    UIManager m_uiOptions;
    bool m_showOptions{false};

    // Audio options
    std::unique_ptr<AudioOptionsPanel> m_audioPanel;
};
