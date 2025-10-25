#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "IScene.h"
#include "ResouceManager.h"
#include "Player.h"

class PlayerTunerScene final : public IScene
{
public:
    explicit PlayerTunerScene(std::string sheetPath);
    ~PlayerTunerScene() override = default;

    void onEnter(Game& game) override;
    void onExit(Game& game) override;
    void handleEvent(Game& game, const sf::Event& ev) override;
    void handleInput(Game& game) override;
    void update(Game& game, float dt) override;
    void draw(Game& game, sf::RenderTarget& rt) override;

private:

    Player::Hand m_activeHand = Player::Hand::Right;
    // --- resources ---
    std::string m_sheetPath;
    ResouceManager m_res;
    Player m_player;

    // --- tuning state ---
    bool  m_showHitbox = true;
    bool  m_showPivotCross = true;

    // Hitbox params (desde pies del player)
    sf::Vector2f m_hbSize   {16.f, 22.f};  // ancho/alto (ajustable)
    sf::Vector2f m_hbOffset { 0.f, -6.f};  // offset desde (pies.x, pies.y)

    // Visual scale
    float m_visualScale = 2.0f;

    // Texto (opcional). Si no carga la fuente, el overlay se omite.
    ResouceManager::FontPtr m_font;
    bool m_fontOk = false;
    sf::Text m_text;
    
    void rebuildOverlay();
    void resetDefaults();

#ifdef _DEBUG
    static void drawCross(sf::RenderTarget& tgt, sf::Vector2f P, sf::Color c = sf::Color::Yellow);
#endif
};
