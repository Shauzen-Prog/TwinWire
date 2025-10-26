#pragma once
#include <SFML/Graphics.hpp>
#include "ResouceManager.h"
#include <optional>
#include "IOrb.h"
#include "SoundManager.h"

class Boss; //fwd

class Orb : public IOrb
{
public:
    Orb(ResouceManager& rm, const std::string& texturePath, sf::Vector2f worldPos, float scale);
    void setSoundManager(SoundManager* sm) { m_sound = sm; }
    
    void setBoss(Boss* b) {m_boss = b;}
    void setActive(bool v) {m_active = v;}

    // IOrb
    bool isActive() const override {return m_active;}
    sf::Vector2f getPosition() const override;
    
    void update(float dt);
    void draw(sf::RenderTarget& rt) const;
    
    sf::FloatRect bounds() const {return m_sprite.getGlobalBounds();}
    sf::Vector2f center() const;

    // Llamado por el pilar cuando cae encima del orbe
    void breakOrb();

private:
    ResouceManager& m_rm;
    ResouceManager::TexturePtr m_tex;
    SoundManager* m_sound{ nullptr };
    sf::Sprite m_sprite;
    
    bool m_active{true};
    float m_respawnTimer{-1.f};
    float m_respawnTime{5.f};
    
    Boss* m_boss{nullptr};
};
