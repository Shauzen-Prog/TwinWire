#pragma once
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>

#include "IScene.h"
#include "ResouceManager.h"
#include "Player.h"
#include "SpriteAnimator.h" // por FrameMeta (solo tipo)

class GameplayScene : public IScene
{
public:
    GameplayScene(std::string sheetPath);
    ~GameplayScene() = default;

    void onEnter(Game& game) override;
    void onExit(Game& game) override;
    void handleEvent(Game& game, const sf::Event& ev) override;
    void handleInput(Game& game) override;
    void update(Game& game, float dt) override;
    void draw(Game& game, sf::RenderTarget& rt) override;
    void rebuildBackgroundForWindow(const sf::RenderWindow& window);
    
private:
    // --- Background ---
    ResouceManager::TexturePtr m_bgTex;
    std::unique_ptr<sf::Sprite> m_bgSprite;
    bool m_bgPixelPerfect = true;
    
    
    std::string m_sheetPath;
    ResouceManager m_res;        
    std::unique_ptr<Player> m_player;
};
