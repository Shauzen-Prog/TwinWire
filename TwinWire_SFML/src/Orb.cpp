#include "Orb.h"
#include "Boss.h"

Orb::Orb(ResouceManager& rm,
         const std::string& texturePath,
         sf::Vector2f worldPos,
         float scale)
: m_rm(rm)
, m_tex(m_rm.getTexture(texturePath))
, m_sprite(*m_tex)
{
    m_tex->setSmooth(false);

    sf::Vector2<float> sz = m_sprite.getLocalBounds().size;
    m_sprite.setOrigin({ sz.x * 0.5f, sz.y * 0.5f });
    m_sprite.setScale({ scale, scale });
    m_sprite.setPosition(worldPos);
}


void Orb::update(float dt)
{
    //cooldown para respawn
    if (!m_active && m_respawnTimer > 0.f) {
        m_respawnTimer -= dt;
        if (m_respawnTimer <= 0.f) {
            m_active = true;
            m_respawnTimer = -1.f; 
        }
    }
}

void Orb::draw(sf::RenderTarget& rt) const
{
    if (m_active)
        rt.draw(m_sprite);
}

sf::Vector2f Orb::center() const
{
    const auto b = m_sprite.getGlobalBounds(); 
    return { b.position.x + b.size.x * 0.5f,
             b.position.y + b.size.y * 0.5f };
}

void Orb::breakOrb()
{
    if (!m_active) return;

    m_active = false;
    m_respawnTimer = m_respawnTime;
}

sf::Vector2f Orb::getPosition() const
{
    return center();
}
