#include "SpriteAnimator.h"
#include <algorithm> // std::min
#include <cmath> // std::fmod

SpriteAnimator::SpriteAnimator(sf::Sprite& sprite)
: m_sprite(sprite){}

void SpriteAnimator::setTexture(const sf::Texture& sheet) {
    m_sprite.setTexture(sheet);
}

void SpriteAnimator::setFrames(std::vector<FrameMeta> frames, bool loop) {
    m_frames = std::move(frames);
    m_loop = loop;
    m_index = 0;
    m_time  = 0.f;
    m_paused = m_frames.empty();
    // Nota: mantener las flags de fin que el caller haya seteado antes
    if (!m_paused) applyCurrentFrameInternal();
}

void SpriteAnimator::update(float dt) {
    if (m_paused || m_frames.empty()) return;

    m_time += dt;
    
    while (m_time >= m_frames[m_index].duration) {
        m_time -= m_frames[m_index].duration;

        // Avanza si hay siguiente
        if (m_index + 1 < static_cast<int>(m_frames.size())) {
            ++m_index;
            applyCurrentFrameInternal();
            continue;
        }

        // Llegamos al final
        const int last = static_cast<int>(m_frames.size()) - 1;
        
        if (m_loop) {
            m_index = 0;
            applyCurrentFrameInternal();
        } else if (m_loopLastFrame)
        {
            // Quedarse "vivo" sobre el ultimo frame sin pausar, por si quiero animar algo externo
            m_index = last;
            // el tiempo puede quedar acotado a la duracion del ultimo frame
            if (m_frames[last].duration > 1e-6f)
                m_time = std::fmod(m_time, m_frames[last].duration); // el fmod me da el resto, seria como un clamp, algo parecido
            applyCurrentFrameInternal();
            // sigue sin pausar para permitir cambios externos inmediatos
        } else if (m_holdOnEnd)
        {
            // Posado en el ultimo frame, queda fijo hasta que cambie de anim, o se llame a resumeFromHold()
            // Mayormente lo hice para el momento del pause con el "Esc", asi no siguen las animaciones corriendo
            m_index = last;
            applyCurrentFrameInternal();
            m_paused = true;
        } else{
            // Fin y pausa (default)
            m_paused = true;
        }
        break;
    }
}

int SpriteAnimator::current() const
{
    return m_index;
}

int SpriteAnimator::frameCount() const
{
    return static_cast<int>(m_frames.size());
}

void SpriteAnimator::applyCurrentFrameInternal()
{
    const FrameMeta& frame = m_frames[m_index];
    const sf::IntRect& rect = frame.rect;
    m_sprite.setTextureRect(rect);
    // pivotX = pixeles desde left; pivotY = base del rect
    m_sprite.setOrigin({frame.pivotX, static_cast<float>(rect.size.y) });
}

void SpriteAnimator::applyCurrentFrame()
{
    applyCurrentFrameInternal();
}

void SpriteAnimator::playOnceHoldLast(const std::vector<FrameMeta>& frames)
{
    setFrames(frames, true); // copia interna si hace falta
    m_holdOnEnd = true;
    m_loopLastFrame = false;
}

