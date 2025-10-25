#pragma once
#include "IScene.h"
#include "ResouceManager.h"

class WinScene final : public IScene
{
public:
    explicit WinScene(ResouceManager& rm);
    
    void onEnter(Game& game) override;
    void onExit(Game& game) override;
    void handleEvent(Game& game, const sf::Event& ev) override;
    void handleInput(Game& game) override;
    void update(Game& game, float dt) override;
    void draw(Game& game, sf::RenderTarget& rt) override;

private:
    ResouceManager& m_rm;
    std::shared_ptr<sf::Font> m_font;

    sf::Text m_title;
    sf::Text m_stats;
    sf::Text m_prompt;
    sf::Text m_initialsText;

    std::string m_initials; // max 3
    bool m_saved{false};

    void refreshText();
};
