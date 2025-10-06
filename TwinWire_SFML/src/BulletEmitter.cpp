#include "BulletEmitter.h"
#include "ResouceManager.h"

BulletEmitter::BulletEmitter(BulletPool& pool,
                       ResouceManager& res,
                       const std::string& texturePath,
                       sf::Vector2f spriteScale)
: m_pool(pool)
{
    m_tex = res.getTexture(texturePath);
    m_scale = spriteScale;
}

void BulletEmitter::emit(sf::Vector2f pos, float angleRad, float speed)
{
    if (Bullet* b = m_pool.acquire()) {
        b->pos = pos;
        b->vel = { std::cos(angleRad) * speed, std::sin(angleRad) * speed };
        b->radius  = 4.f;
        b->maxLife = 2.0f;

        if (m_tex) {
            b->sprite = std::make_unique<sf::Sprite>(*m_tex);  // crear el sprite
            b->sprite->setScale(m_scale);
            b->sprite->setPosition(b->pos);

            const auto lb = b->sprite->getLocalBounds(); 
            b->sprite->setOrigin(lb.position + (lb.size * 0.5f));
        } else {
            b->sprite.reset(); // sin textura crea un círculo
        }
    }
}
