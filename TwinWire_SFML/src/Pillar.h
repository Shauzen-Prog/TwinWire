#pragma once
#include "SFML/Graphics.hpp"
#include "ResouceManager.h"
#include "IChockeable.h"

class Orb;

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

    //Orbe que colisiona

    // IChockeable
    void onChoke(const sf::Vector2f& aTip, const sf::Vector2f& bTip) override;
    
    void update(float dt);
    void draw(sf::RenderTarget& rt) const override { if (m_active || m_isFalling) rt.draw(m_sprite); }

    
    sf::FloatRect bounds() const override { return m_sprite.getGlobalBounds(); }
    
    // <summary> Config: duración total de la caída y el punto de impacto (0..1)</summary>
    void setFallParams(float durationSeconds, float impactAtNormalized)
    {
        m_fallDuration = (durationSeconds > 0.f) ? durationSeconds : 0.01f;
        m_impactAt     = std::clamp(impactAtNormalized, 0.f, 1.f);
    }

    // <summary> respawn: -1 desactiva, cualquier valor >=0 programa el respawn </summary>
    void setRespawnSeconds(float seconds) { m_respawnAfter = seconds; }

    // <summary>Forzar angulo final en el suelo (por defecto 90°). Se cambia el signo para caer hacia un lado.</summary>
    void setGroundAngleDeg(float deg) { m_groundAngleDeg = deg; }

    // <summary>Set del callback a disparar en el impacto.</summary>
    template <class F>
    void setOnImpact(F&& f) { m_onImpact = std::forward<F>(f); }

private:

    // --- data render ---
    ResouceManager& m_rm;
    ResouceManager::TexturePtr m_tex;
    sf::Sprite m_sprite;
    
    // --- estado logico ---
    bool m_active{true}; // puede recibir el choke
    bool m_shouldDraw{true}; // control visual (desaparece al terminar
    bool m_isFalling{false};
    bool m_hasImpacted{false};

    // --- tween de caida ---
    float m_fallDuration{0.6f}; // total de la animacion (s)
    float m_fallT{0.f}; // [0..1]
    float m_impactAt{ 0.35f}; // cuando "pega" (0..1)
    float m_rotStartDeg{0.f};
    float m_rotEndDeg{90.f}; // al ras del piso (90°) cambio el signo para caer a otro lado
    float m_groundAngleDeg{90.f};
    

    // --- Respawn ---
    float m_respawnAfter { -1.f}; // -1 = sin respawn, >=0 segundos para reaparecer
    float m_respawnTimer {-1.f};

    // --- Callback de impacto ---
    std::function<void()> m_onImpact;
    
    // helpers
    static float easeOutCubic(float t) { return 1.f - (1.f - t)*(1.f - t)*(1.f - t); }
    void begin_fall();
    void finishFall_();
    void scheduleRespawn_();
};
