#pragma once
#include "IWidget.h"
#include "UIButton.h"
#include <memory>
#include <vector>

class UIManager
{
public:
    UIManager() = default;
    ~UIManager() = default;

    // Crea y guarda un boton. Devuelve referencia para configurar callbacks
    UIButton& createButton(
        const sf::Font& font,
        const std::string& label,
        sf::Vector2f centerPos,
        sf::Vector2f size);

    void handleEvent(const sf::Event& ev, const sf::RenderWindow& window);

    void update(float dt);

    void draw(sf::RenderTarget& target) const;

private:
    // por ahora solo almanceno botones. Después puedo agregar mas cosas 
    std::vector<std::unique_ptr<IWidget>> m_buttons;
};
