#pragma once
#include "IScene.h"
#include "ResouceManager.h"
#include "UIManager.h"

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
    ResouceManager res;
    
    sf::Text m_title;
    ResouceManager::FontPtr m_font;

    UIManager m_ui;
};
