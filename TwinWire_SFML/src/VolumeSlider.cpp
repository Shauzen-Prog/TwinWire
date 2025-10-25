#include "VolumeSlider.h"

VolumeSlider::VolumeSlider()
{
    m_bar.setFillColor(sf::Color(60,60,60));
    m_fill.setFillColor(sf::Color(180,180,220));
    m_knob.setRadius(8.f);
    m_knob.setOrigin({8.f,8.f});
    m_knob.setFillColor(sf::Color(230,230,255));
    refreshVisual();
}

void VolumeSlider::setRect(const sf::FloatRect& r)
{
    m_rect = r;
    refreshVisual();
}

void VolumeSlider::setValue(float v)
{
    m_value = (v < 0.f) ? 0.f : (v > 1.f ? 1.f : v);
    refreshVisual();
}

void VolumeSlider::refreshVisual()
{
    // Barra
    m_bar.setPosition(m_rect.position);
    m_bar.setSize({ m_rect.size.x, m_rect.size.y });

    // Relleno
    const float w = m_rect.size.x * m_value;
    m_fill.setPosition(m_rect.position);
    m_fill.setSize({ w, m_rect.size.y });

    // Knob
    const float x = m_rect.position.x + w;
    const float y = m_rect.position.y + m_rect.size.y * 0.5f;
    m_knob.setPosition({ x, y });
}

void VolumeSlider::handleEvent(const sf::Event& ev, const sf::RenderWindow& window)
{
    if (const auto* mp = ev.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            const sf::Vector2i pix = sf::Mouse::getPosition(window);
            const sf::Vector2f p   = window.mapPixelToCoords(pix);
            if (m_rect.contains(p)) {
                m_drag = true;
                const float t = (p.x - m_rect.position.x) / m_rect.size.x;
                setValue(t);
            }
        }
    }
    if (const auto* mr = ev.getIf<sf::Event::MouseButtonReleased>()) {
        if (mr->button == sf::Mouse::Button::Left) m_drag = false;
    }
    if (ev.is<sf::Event::MouseMoved>() && m_drag) {
        const sf::Vector2i pix = sf::Mouse::getPosition(window);
        const sf::Vector2f p   = window.mapPixelToCoords(pix);
        const float t = (p.x - m_rect.position.x) / m_rect.size.x;
        setValue(t);
    }
}

void VolumeSlider::update(float /*dt*/)
{
    // (sin animaciones por ahora)
}

void VolumeSlider::draw(sf::RenderTarget& target) const
{
    target.draw(m_bar);
    target.draw(m_fill);
    target.draw(m_knob);
}