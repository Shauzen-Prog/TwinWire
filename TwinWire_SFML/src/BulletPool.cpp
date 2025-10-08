#include "BulletPool.h"
#include <algorithm>

BulletPool::BulletPool(std::size_t capacity)
: m_items(capacity) {}

Bullet* BulletPool::acquire()
{
    for (auto& b : m_items)
    {
        if (!b.active)
        {
            b.active = true;
            b.textured = false; // por las dudas
            b.life = 0.f;
            ++m_active;
            return &b;
        }
    }
    return nullptr; // lleno
}

void BulletPool::update(float dt, const sf::FloatRect& cullRect)
{
    // Movimiento + vida + culling
    for (auto& b : m_items) if (b.active) {
        b.pos  += b.vel * dt;
        b.life += dt;

        if (b.sprite) // si existe sprite
            b.sprite->setPosition(b.pos);

        const bool out = (b.life >= b.maxLife) || !cullRect.contains(b.pos);
        
        if (out) { b.active = false; --m_active; b.sprite.reset(); }
    }
}

void BulletPool::update(float dt,
                        const sf::FloatRect& cullRect,
                        const sf::FloatRect& playerAABB,
                        bool& outPlayerHit)
{
    outPlayerHit = false;

    for (auto& b : m_items) if (b.active) {
        // mover
        b.pos  += b.vel * dt;
        b.life += dt;

        if (b.sprite) b.sprite->setPosition(b.pos);

        // colision simple
        if (playerAABB.contains(b.pos)) {
            b.active = false;
            --m_active;
            b.sprite.reset();
            outPlayerHit = true;        // avisa que pegó
            continue;                   // pasa a la siguiente bala
        }

        // culling / lifetime
        const bool out = (b.life >= b.maxLife) || !cullRect.contains(b.pos);
        if (out) {
            b.active = false;
            --m_active;
            b.sprite.reset();
        }
    }
}

void BulletPool::draw(sf::RenderTarget& rt) const
{
    for (const auto& b : m_items) if (b.active) {
        if (b.sprite) {
            rt.draw(*b.sprite);  // desreferencia
        } else {
            sf::CircleShape s(b.radius);
            s.setOrigin({b.radius, b.radius});
            s.setPosition(b.pos);
            rt.draw(s);
        }
    }
}

void BulletPool::clear()
{
    for (auto& b : m_items)
    {
        b.active = false;
        b.life = 0.f;
        b.maxLife = 0.f;
        b.pos = {};
        b.vel = {};
        b.sprite.reset();
    }
}




