#pragma once
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <optional>

#include "IScene.h"
#include "Pillar.h"
#include "ResouceManager.h"
#include "Player.h"
#include "SpriteAnimator.h" // por FrameMeta (solo tipo)

class Pillar;             // fwd
struct IChokeQuery;       // fwd

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
    struct MultiPillarQuery;                       //fwd anidada
    std::unique_ptr<MultiPillarQuery> m_query;
    ResouceManager::TexturePtr m_bgTex;
    std::unique_ptr<sf::Sprite> m_bgSprite;
    bool m_bgPixelPerfect = true;
    
    std::vector<std::unique_ptr<Pillar>> m_pillars;
    std::vector<Pillar*> m_livePtrs;

    Pillar& spawnPillar(ResouceManager& rm,
        const std::string& tex,
        std::optional<sf::IntRect> rect,
        sf::Vector2f pos,
        float scale = 1.f);

    std::string m_sheetPath;
    ResouceManager m_res;        
    Player m_player;
};
