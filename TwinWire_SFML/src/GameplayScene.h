#pragma once
#include <memory>
#include <string>
#include <vector>

#include <SFML/Graphics.hpp>
#include <optional>

#include "Boss.h"
#include "IOrb.h"

#include "IChockeable.h"
#include "IScene.h"
#include "Pillar.h"
#include "ResouceManager.h"
#include "Player.h"
#include "SpriteAnimator.h" // por FrameMeta (solo tipo)
#include "BulletPool.h"
#include "BulletEmitter.h"


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
    std::vector<std::function<void()>> m_deferred; // tareas para ejecutar post-update
    
private:

    void resetAll(); // helper para dejar como recien creado
        
    // --- Background ---
    struct MultiPillarQuery;                       //fwd anidada
    std::unique_ptr<MultiPillarQuery> m_query;
    ResouceManager::TexturePtr m_bgTex;
    std::unique_ptr<sf::Sprite> m_bgSprite;
    bool m_bgPixelPerfect = true;
    
    sf::FloatRect m_playerAABB{}; 

    std::vector<std::unique_ptr<Orb>> m_orbs; //lista de orbes
    std::vector<IChockeable*> m_liveChoke;

    Orb& spawnOrb(ResouceManager& rm, const std::string& tex, sf::Vector2f pos, float scale = 1.f);
    
    std::vector<std::unique_ptr<Pillar>> m_pillars;
    std::vector<Pillar*> m_livePtrs;

    Pillar& spawnPillar(ResouceManager& rm,
        const std::string& tex,
        std::optional<sf::IntRect> rect,
        sf::Vector2f pos,
        float scale = 1.f);

    // Bullet API
    BulletPool m_bullets{800}; // capacidad inicial
    std::unique_ptr<BulletEmitter> m_emitter;
    // culling rect (ventana + margen)
    sf::FloatRect m_cullRect{};
    
    std::string m_sheetPath;
    ResouceManager m_res;        
    Player m_player;
    bool m_playerDead = false;
    float m_restarTimer = -1.f;

    std::unique_ptr<Boss> m_boss;
    std::vector<IOrb*> m_orbViews;

#ifdef _DEBUG
    
    bool m_pivotTunerActive = false;
    
#endif
    
};
