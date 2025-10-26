#pragma once
#include "IScene.h"
#include "ResouceManager.h"
#include "UIManager.h"
#include "AudioOptionsPanel.h"
#include <optional>

#include "Boss.h"
#include "BulletPool.h"

class BulletEmitter;

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
    ResouceManager::TexturePtr m_bgTex;
    std::unique_ptr<sf::Sprite> m_bgSprite;
    bool m_bgPixelPerfect{ true };
    ResouceManager::FontPtr m_font;
    sf::Text m_title;
    sf::FloatRect m_cullRect{};
    sf::Text m_best; // <- "Mejor: AJX 3:42 M:2"
    
    std::optional<sf::Text> m_scoresTop;
    
    UIManager m_ui;
    
    UIManager m_uiOptions;
    bool m_showOptions{false};

    // --- Créditos
    UIManager m_uiCredits;
    bool m_showCredits{false};
    sf::Text m_creditsTitle;
    sf::Text m_creditsBody;

    UIManager m_uiHowTo;
    bool m_showHowTo{ false };
    sf::Text m_howToTitle;
    sf::Text m_howToBody;

    sf::FloatRect m_btnPlayRect{};
    sf::FloatRect m_btnHowToRect{};
    sf::FloatRect m_btnOptionsRect{};
    sf::FloatRect m_btnCreditsRect{};
    sf::FloatRect m_btnExitRect{};
    
    void rebuildBackgroundForWindow(const sf::RenderWindow& window);
    void buildCreditsTexts();
    void buildHowToTexts();

    std::unique_ptr<Boss> m_boss;
    std::unique_ptr<BulletEmitter> m_emitter;
    BulletPool m_bullets{300};
    sf::Clock m_bossTimer;
    
    // Audio options
    std::unique_ptr<AudioOptionsPanel> m_audioPanel;
};
