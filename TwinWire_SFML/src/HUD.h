#pragma once
#include <SFML/Graphics/Drawable.hpp>
#include <memory>
#include "ResouceManager.h"

class HUD final : public sf::Drawable
{
public:
    explicit HUD(ResouceManager& rm);

    void setViewport(const sf::RenderWindow& win); // para reposicionar UI si cambia de tamaño
    void update(float dt);
    
private:
    std::shared_ptr<sf::Font> m_font;
    sf::Text m_text;

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
