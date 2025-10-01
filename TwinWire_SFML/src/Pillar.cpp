#include "Pillar.h"

Pillar::Pillar(ResouceManager& rm,
               const std::string& texturePath,
               std::optional<sf::IntRect> rect,
               sf::Vector2f worldPos,
               float scale)
: m_rm(rm)
, m_tex(m_rm.getTexture(texturePath)) 
, m_sprite(*m_tex)   
{
    m_tex->setSmooth(true);
    
    if (rect) {
        m_sprite.setTextureRect(*rect);
    }
    
    const sf::Vector2f sz = m_sprite.getLocalBounds().size;

    // Pivote: centro X, base Y
    m_sprite.setOrigin({ sz.x * 0.5f, sz.y });
    m_sprite.setScale({ scale, scale });
    m_sprite.setPosition(worldPos);

    
}

void Pillar::onChoke(const sf::Vector2f&, const sf::Vector2f&)
{
    m_active = false;
    m_respawnTimer = 3.f; // vuelve en 3s
}

void Pillar::update(float dt)
{
    if (m_respawnTimer >= 0.f) {
        m_respawnTimer -= dt;
        if (m_respawnTimer <= 0.f) {
            m_active = true;
            m_respawnTimer = -1.f;
        }
    }
}
