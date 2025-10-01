#include "UIButton.h"
#include <SFML/Window.hpp>

UIButton::UIButton(const sf::Font& font,
                   const std::string& label,
                   sf::Vector2f center,
                   sf::Vector2f size)
: m_text(font, label, static_cast<unsigned int>(size.y * 0.45f)) 
{
    m_box.setSize(size);
    m_box.setOrigin(size * 0.5f);
    m_box.setPosition(center);
    m_box.setFillColor(m_colNormal);
    m_box.setOutlineThickness(2.f);
    m_box.setOutlineColor(sf::Color(220, 220, 220, 100));

    m_text.setFont(font);
    m_text.setString(label);
    m_text.setCharacterSize(static_cast<unsigned int>(size.y * 0.45f));
    m_text.setFillColor(sf::Color::White);
    
    centerText();
}

void UIButton::handleEvent(const sf::Event& ev, const sf::RenderWindow& window)
{
    if (const auto* mm = ev.getIf<sf::Event::MouseMoved>())
    {
        const sf::Vector2i px{ mm->position.x, mm->position.y };
        const sf::Vector2f world = window.mapPixelToCoords(px);
        m_hovered = containsPoint(world);
        return;
    }
    
    if (const auto* mp = ev.getIf<sf::Event::MouseButtonPressed>())
    {
        if (mp->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f world = window.mapPixelToCoords({ mp->position.x, mp->position.y });
            if (containsPoint(world))
                m_pressed = true;
        }
        return;
    }
    
    if (const auto* mr = ev.getIf<sf::Event::MouseButtonReleased>())
    {
        if (mr->button == sf::Mouse::Button::Left)
        {
            const sf::Vector2f world = window.mapPixelToCoords({ mr->position.x, mr->position.y });
            const bool inside = containsPoint(world);
            if (m_pressed && inside && m_onClick) m_onClick();
            m_pressed = false; // reset
        }
        return;
    }
}

void UIButton::update(float /*dt*/)
{
    // Simple visual state
    if (m_pressed)      m_box.setFillColor(m_colPressed);
    else if (m_hovered) m_box.setFillColor(m_colHover);
    else                m_box.setFillColor(m_colNormal);
}

void UIButton::draw(sf::RenderTarget& target) const
{
    target.draw(m_box);
    target.draw(m_text);
}

void UIButton::setLabel(const std::string& text)
{
    m_text.setString(text);
    centerText();
}

void UIButton::setCenter(sf::Vector2f center)
{
    m_box.setPosition(center);
    centerText();
}

void UIButton::setSize(sf::Vector2f size)
{
    m_box.setSize(size);
    m_box.setOrigin(size * 0.5f);
    m_text.setCharacterSize(static_cast<unsigned int>(size.y * 0.45f));
    centerText();
}

void UIButton::setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& pressed)
{
    m_colNormal = normal;
    m_colHover = hover;
    m_colPressed = pressed;
}

void UIButton::setTextColor(const sf::Color& color)
{
    m_text.setFillColor(color);
}

bool UIButton::containsPoint(const sf::Vector2f& p) const
{
    const auto b = m_box.getGlobalBounds();
    return b.contains(p);
}

void UIButton::centerText()
{
    const auto b = m_text.getLocalBounds();
    m_text.setOrigin({ b.position.x + b.size.x * 0.5f, b.position.y + b.size.y * 0.5f });
    m_text.setPosition(m_box.getPosition());
}

