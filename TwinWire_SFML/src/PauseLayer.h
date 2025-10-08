#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include "UIManager.h"

class PauseLayer
{
public:
    PauseLayer() = default;

    void build(const sf::Font& font, const sf::Vector2u& windowSize);
    void handleEvent(const sf::Event& ev, const sf::RenderWindow& window);
    void update(float dt);
    void draw(sf::RenderTarget& target) const;

    // Visibilidad
    bool isVisible() const {return m_visible;}
    void show() {m_visible = true;}
    void hide() {m_visible = false;}
    void toggle() {m_visible = !m_visible;}

    // Callbacks
    void setOnResume(std::function<void()> cb)  { m_onResume  = std::move(cb); }
    void setOnOptions(std::function<void()> cb) { m_onOptions = std::move(cb); }
    void setOnExit(std::function<void()> cb)    { m_onExit    = std::move(cb); }


private:
    bool m_visible{false};
    UIManager m_ui;
    sf::RectangleShape m_dim;

    // CallBacks de los botones
    std::function<void()> m_onResume;
    std::function<void()> m_onOptions;
    std::function<void()> m_onExit;
    
};
