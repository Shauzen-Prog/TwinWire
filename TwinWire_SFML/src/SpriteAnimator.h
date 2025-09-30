#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "FrameMeta.h"

class SpriteAnimator
{
public:
    explicit SpriteAnimator(sf::Sprite& sprite);

    void setTexture(const sf::Texture& sheet);
    void setFrames(std::vector<FrameMeta> frames, bool loop = true);
    void update(float dt);

    int  current() const;
    int  frameCount() const;

private:
    void applyCurrentFrame();

private:
    sf::Sprite& m_sprite;
    std::vector<FrameMeta> m_frames;
    int m_index = 0;
    float m_time  = 0.f;
    bool m_loop  = true;
    bool m_paused = false;
};
