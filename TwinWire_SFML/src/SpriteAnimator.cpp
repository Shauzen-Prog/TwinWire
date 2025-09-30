#include "SpriteAnimator.h"

SpriteAnimator::SpriteAnimator(sf::Sprite& sprite)
: m_sprite(sprite)
{
}

void SpriteAnimator::setTexture(const sf::Texture& sheet) {
    m_sprite.setTexture(sheet);
}

void SpriteAnimator::setFrames(std::vector<FrameMeta> frames, bool loop) {
    m_frames = std::move(frames);
    m_loop = loop;
    m_index = 0;
    m_time  = 0.f;
    m_paused = m_frames.empty();
    if (!m_paused) applyCurrentFrame();
}

void SpriteAnimator::update(float dt) {
    if (m_paused || m_frames.empty()) return;

    m_time += dt;
    while (m_time >= m_frames[m_index].duration) {
        m_time -= m_frames[m_index].duration;
        // avanzar de frame
        if (m_index + 1 < static_cast<int>(m_frames.size())) {
            ++m_index;
        } else {
            if (m_loop) m_index = 0;
            else { m_paused = true; break; }
        }
        applyCurrentFrame();
        if (m_paused) break;
    }
}

int SpriteAnimator::current() const {
    return m_index;
}

int SpriteAnimator::frameCount() const {
    return static_cast<int>(m_frames.size());
}

void SpriteAnimator::applyCurrentFrame() {
    const FrameMeta& f = m_frames[m_index];
    m_sprite.setTextureRect(f.rect);
    // Pivot “talón”: X medido por frame, Y = base del rect
    m_sprite.setOrigin({f.pivotX, static_cast<float>(f.rect.size.y)});
}
