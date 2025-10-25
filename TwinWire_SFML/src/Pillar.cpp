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
    
    const sf::Vector2f sz = static_cast<sf::Vector2f>(m_sprite.getTextureRect().size);
    m_sprite.setOrigin({sz.x * 0.5f, sz.y }); //centro en X y base en Y
    m_sprite.setPosition(worldPos);
    m_sprite.setRotation(sf::Angle::Zero);
    m_sprite.setScale({ scale, scale });

    // Defaults
    m_fallDuration = 0.6f;
    m_impactAt = 0.35f;
    m_groundAngleDeg = 90.f;
    m_respawnAfter = -1.f;
    
}

void Pillar::onChoke(const sf::Vector2f&, const sf::Vector2f&)
{
    if (!m_active || m_isFalling) return;

    // se desactiva para no recibir otro choke pero se sigue dibujando
    m_active     = false;
    m_hasImpacted = false;
    begin_fall();
}

void Pillar::begin_fall()
{
    m_isFalling   = true;
    m_fallT       = 0.f;
    m_rotStartDeg = m_sprite.getRotation().asDegrees();
    m_rotEndDeg   = m_groundAngleDeg; // 90° o -90°, configurable

    // feedback rapido
    m_sprite.setColor(sf::Color(255, 220, 220));
}

void Pillar::finishFall_()
{
    // queda al ras del piso
    m_sprite.setRotation(sf::degrees(m_rotEndDeg));
    m_sprite.setColor(sf::Color::White);

    // desaparece visualmente
    m_shouldDraw = false;
    m_isFalling  = false;

    scheduleRespawn_();
}

void Pillar::scheduleRespawn_()
{
    if (m_respawnAfter >= 0.f) {
        m_respawnTimer = m_respawnAfter;
    }
}

void Pillar::update(float dt)
{
    // caída animada
    if (m_isFalling)
    {
        if (m_fallDuration <= 1e-4f) m_fallDuration = 0.01f;
        m_fallT = std::min(1.f, m_fallT + dt / m_fallDuration);

        const float t = easeOutCubic(m_fallT);
        const float ang = m_rotStartDeg + (m_rotEndDeg - m_rotStartDeg) * t;
        m_sprite.setRotation(sf::degrees(ang));

        // Momento de impacto (una sola vez)
        if (!m_hasImpacted && m_fallT >= m_impactAt)
        {
            m_hasImpacted = true;
            if (m_onImpact) m_onImpact(); // romper orbe, sfx, screenshake, etc.
        }

        if (m_fallT >= 1.f)
            finishFall_();
    }

    // respawn
    if (m_respawnTimer >= 0.f)
    {
        m_respawnTimer -= dt;
        if (m_respawnTimer <= 0.f)
        {
            m_respawnTimer = -1.f;

            // reaparece en estado original
            m_active     = true;
            m_shouldDraw = true;
            m_isFalling  = false;
            m_hasImpacted= false;

            m_sprite.setRotation(sf::Angle::Zero);
            m_sprite.setColor(sf::Color::White);
        }
    }
}


