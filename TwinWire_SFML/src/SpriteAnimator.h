#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "FrameMeta.h"

static FrameMeta F(int x, int y, int w, int h, float px, float dur=0.08f) {
    return { sf::IntRect{{x,y},{w,h}}, px, dur };
}

class SpriteAnimator
{
public:
    explicit SpriteAnimator(sf::Sprite& sprite);

    // Cargar textura y frames
    void setTexture(const sf::Texture& sheet);
    void setFrames(std::vector<FrameMeta> frames, bool loop = true);
    void update(float dt);

    // Info del frame actual
    int current() const;
    int frameCount() const;
    const sf::IntRect& currentRect() const {return m_frames[m_index].rect; }
    float currentPivotX() const { return m_frames[m_index].pivotX; }
    float currentFrameDuration() const { return m_frames[m_index].duration; }

    // Ajuste fino de pivotX (para el tuner)
    void nudgePivotX(float dx) {
        m_frames[m_index].pivotX += dx;
        applyCurrentFrame(); // ya setea origin = (pivotX, rect.height)
    }

    // Acceso al sprite (para dibujar pivot / sacar transform)
    const sf::Sprite& sprite() const { return m_sprite; }
    sf::Sprite& sprite() { return m_sprite; }

    // Acceso a todos los frames (para guardar/cargar CSV)
    const std::vector<FrameMeta>& frames() const { return m_frames; }
    std::vector<FrameMeta>& framesMutable() { return m_frames; }

    // Reaplica rect/origin del frame actual
    void applyCurrentFrame();

    // --- Comportamientos de fin de anim ---
    // Reproducir una vez y QUEDARSE en el ultima frame
    void playOnceHoldLast(const std::vector<FrameMeta>& frames);

    // Seteo explicito por si lo armo con setFrames
    void setHoldOnEnd(bool hold) { m_holdOnEnd = hold; }
    // Loopear solo el ultimo frame ( no se pause, queda en el ultimo hasta que se cambie de anim)
    void loopLastFrame(bool v) { m_loopLastFrame = v; if (v) m_holdOnEnd = false; }

    // Control del pause si se queda holdeado
    void resumeFromHold() { m_paused = false; m_time = 0.f; } // para “destrabar”
    void setPaused(bool p) { m_paused = p; }

    // Getter de pausa
    bool isPaused() const { return m_paused; }

private:
    
    sf::Sprite& m_sprite;
    std::vector<FrameMeta> m_frames;
    bool m_holdOnEnd = false; // si termina y NO loopea -> queda en el ultimo y se pausa
    bool m_loopLastFrame = false; // ignora el hold: reitera el ultimo frame -> sin pausa
    int   m_index   = 0;
    float m_time    = 0.f;
    bool  m_loop    = true;
    bool  m_paused  = false;
    
    void applyCurrentFrameInternal();
};
