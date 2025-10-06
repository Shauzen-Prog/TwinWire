#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <cmath>
#include "IBulletEmitter.h"
#include "BulletPool.h"
#include "ResouceManager.h"

class BulletEmitter final : public IBulletEmitter
{
public:
    explicit BulletEmitter(BulletPool& pool,
                            ResouceManager& res,
                            const std::string& texturePath,
                            sf::Vector2f spriteScale = {1.f,1.f});

    void emit(sf::Vector2f pos, float angleRad, float speed) override;

private:
    BulletPool& m_pool;
    ResouceManager::TexturePtr m_tex{};
    sf::Vector2f m_scale{1.f, 1.f};  
};
