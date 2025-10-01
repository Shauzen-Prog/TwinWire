#pragma once
#include "IWidget.h"
#include <functional>
#include <string>

// Simple rectangular button with a centered label
class UIButton final : public IWidget
{
public:
    UIButton(const sf::Font& font,
             const std::string& label,
             sf::Vector2f center,
             sf::Vector2f size);

    // IWidget
    void handleEvent(const sf::Event& ev, const sf::RenderWindow& window) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;

    // API
    void setOnClick(std::function<void()> cb) { m_onClick = std::move(cb); }
    void setLabel(const std::string& text);
    void setCenter(sf::Vector2f center);
    void setSize(sf::Vector2f size);

    // Styling
    void setColors(const sf::Color& normal, const sf::Color& hover, const sf::Color& pressed);
    void setTextColor(const sf::Color& color);

private:
    sf::RectangleShape m_box;
    sf::Text m_text;

    bool m_hovered{false};
    bool m_pressed{false};
    bool m_enabled{true};

    sf::Color m_colNormal{ sf::Color(30, 34, 40) };
    sf::Color m_colHover{ sf::Color(45, 110, 210) };
    sf::Color m_colPressed{ sf::Color(25, 80, 170) };

    std::function<void()> m_onClick;

    bool containsPoint(const sf::Vector2f& worldPoint) const;
    void centerText();
};
