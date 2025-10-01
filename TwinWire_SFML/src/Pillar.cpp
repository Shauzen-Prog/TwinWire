#include "Pillar.h"
#include "Orb.h"

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
    if (!m_active) return;

    // Arranca caida (temporal por tiempo me gustaría hacerlo con animacion creo que es mas robusto)
    m_isFalling = true;
    m_fallTimer = m_fallDelay;
    m_sprite.setColor(sf::Color(255, 200, 200));
    
    m_active = false;
    m_respawnTimer = 3.f; // vuelve en 3s

}

void Pillar::update(float dt)
{
    // --- si esta cayendo por tiempo ---
    if (m_isFalling)
    {
        if (m_fallTimer >= 0.f)
        {
            m_fallTimer -= dt;

            //animacion barata de momento
            m_sprite.rotate(sf::degrees(45.f * dt));

            if (m_fallTimer <= 0.f) {
                // toco el orbe: lo rompe si existe y esta activo
                if (m_targetOrb && m_targetOrb->isActive())
                {
                    m_targetOrb->breakOrb(); // pierde robuztes por que esta acoplado, podría hacerlo con un listener o por evento con evenbus
                }

                // se apaga el pilar y entra en respawn
                m_active = false;
                m_isFalling = false;
                m_fallTimer = -1.f;
                m_sprite.setColor(sf::Color::White);

                // esa el respawn por defecto
                respawnIn(3.f);
                
            }
        }
    }
    
    // --- respawn estándar del pilar ---
    if (m_respawnTimer >= 0.f) {
        m_respawnTimer -= dt;
        if (m_respawnTimer <= 0.f) {
            m_active = true;
            m_respawnTimer = -1.f;

            // reset visual
            m_sprite.setRotation(sf::Angle::Zero);
            m_sprite.setColor(sf::Color::White);
        }
    }
}
