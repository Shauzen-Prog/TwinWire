#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "Bullet.h"

// prodria hacer una pool generica pero voy a hacer una especifica
// porque mas adelante no voy a usar pool para ninguna otra cosa
class BulletPool 
{
public:
    explicit BulletPool(std::size_t size);

    Bullet* acquire(); //nullprt si lleno
    void update(float dt, const sf::FloatRect& cullRect);
    void update(float dt,
                const sf::FloatRect& cullRect,
                const sf::FloatRect& playerAABB,
                bool& outPlayerHit);
    
    void draw(sf::RenderTarget& rt) const;

    int activeCount() const { return m_active; }
    int capacity() const { return static_cast<int>(m_items.size()); }

private:
    std::vector<Bullet> m_items;
    int m_active{0}; // contado al vuelo (hint perf)
};
