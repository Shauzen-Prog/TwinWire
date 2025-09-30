#include "UIManager.h"

UIButton& UIManager::createButton(const sf::Font& font, const std::string& label, sf::Vector2f centerPos,
    sf::Vector2f size)
{
    m_buttons.emplace_back(std::make_unique<UIButton>(font, label, centerPos, size));

    auto* raw = static_cast<UIButton*>(m_buttons.back().get()); // sabemos que es un UIButton
    return *raw;
}

void UIManager::handleEvent(const sf::Event& ev, const sf::RenderWindow& window)
{
    for (std::unique_ptr<IWidget>& w : m_buttons) w->handleEvent(ev, window);
}

void UIManager::update(float dt)
{
    for (std::unique_ptr<IWidget>& w : m_buttons) w->update(dt);
}

void UIManager::draw(sf::RenderTarget& target) const
{
    for (const std::unique_ptr<IWidget>& w : m_buttons) w->draw(target);
}

