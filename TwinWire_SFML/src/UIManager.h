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

    // Deleteo copia y lo dejo en noexcept para que se pueda tranferir propiedad sin copiar
    // Esto por que como tiene un unique_ptr no pueden tener 2 lo mismo, que es lo que hacia que se me rompa
    UIManager(const UIManager&) = delete; // no copiable
    UIManager& operator=(const UIManager&) = delete; // no copiable
    
    UIManager(UIManager&&) noexcept = default; // movible
    UIManager& operator=(UIManager&&) noexcept = default; // movible

    void clear() { m_buttons.clear(); }
    
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
