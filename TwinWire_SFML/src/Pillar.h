#pragma once
#include "SFML/Graphics.hpp"
#include "ResouceManager.h"
#include "IChockeable.h"

class Pillar : public IChockeable
{
public:
    Pillar(ResouceManager& rm,
           const std::string& texturePath,
           std::optional<sf::IntRect> rect,
           sf::Vector2f worldPos,
           float scale = 1.f);

    void setActive(bool v) { m_active = v; }
    bool isActive() const  { return m_active; }

    // IChockeable
    void onChoke(const sf::Vector2f&, const sf::Vector2f&) override;
    sf::FloatRect bounds() const override { return m_sprite.getGlobalBounds(); }
    void update(float dt);
    void draw(sf::RenderTarget& rt) const override { if (m_active) rt.draw(m_sprite); }

    void respawnIn(float seconds) { m_respawnTimer = seconds; }

private:
    ResouceManager& m_rm;
    ResouceManager::TexturePtr m_tex;
    sf::Sprite m_sprite;
    bool m_active{true};

    float m_respawnTimer{-1.f};
};
