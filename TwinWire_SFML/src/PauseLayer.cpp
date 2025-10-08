#include "PauseLayer.h"

void PauseLayer::build(const sf::Font& font, const sf::Vector2u& windowSize)
{
    // Overlay oscuro
    m_dim.setSize({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    m_dim.setFillColor(sf::Color(0, 0, 0, 180));
    
    m_ui.clear();

    const float cx = static_cast<float>(windowSize.x) * 0.5f;
    float cy = windowSize.y * 0.5f - 65.f;
    const sf::Vector2f btnSize{ 240.f, 60.f };

    auto styleButton = [](UIButton& b)
    {
        // mismo esquema de colores que en MainMenu
        b.setColors(
            sf::Color(88, 76, 140),   // normal
            sf::Color(120, 76, 140),  // hover
            sf::Color(96, 140, 76)    // pressed
        );
        b.setTextColor(sf::Color(230, 230, 230));
    };
    
    // Resume
    UIButton& bResume = m_ui.createButton(font, "Reanudar", {cx, cy}, btnSize);
    styleButton(bResume);
    bResume.setOnClick([this]() { if (m_onResume)  m_onResume(); });
    cy += 120.f;// distancia

    // Options
    UIButton& bOptions = m_ui.createButton(font, "Opciones", {cx, cy}, btnSize);
    styleButton(bOptions);
    bOptions.setOnClick([this]() { if (m_onOptions) m_onOptions(); });
    cy += 120.f;

    // Exit
    UIButton& bExit = m_ui.createButton(font, "Salir al menu", {cx, cy}, btnSize);
    styleButton(bExit);
    bExit.setOnClick([this]() { if (m_onExit)    m_onExit(); });
    
}

void PauseLayer::handleEvent(const sf::Event& ev, const sf::RenderWindow& window)
{
    if (!m_visible) return;
    m_ui.handleEvent(ev, window);
}

void PauseLayer::update(float dt)
{
    if (!m_visible) return;
    m_ui.update(dt);
}

void PauseLayer::draw(sf::RenderTarget& target) const
{
    if (!m_visible) return;

    const sf::View old = target.getView();
    target.setView(target.getDefaultView()); 

    target.draw(m_dim);
    m_ui.draw(target);

    target.setView(old);
}
