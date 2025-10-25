#include "HUD.h"
#include "RunStats.h"
#include "iomanip"
#include "sstream"

static std::string formatTimeMMSS(float t)
{
    int total = static_cast<int>(t + 0.5f);
    int m = total / 60;
    int s = total % 60;
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
    return std::string(buf);
}

HUD::HUD(ResouceManager& rm):
m_font{ rm.getFont("../../../../res/Assets/Fonts/Jersey25-Regular.ttf")}
, m_text( *m_font, "Tiempo 00:00   Muertes 0", 32u)
{
    m_text.setFillColor(sf::Color::White);
    m_text.setOutlineColor(sf::Color::Black);
    m_text.setOutlineThickness(2.f);
    m_text.setPosition({10.f, 8.f});
}

void HUD::setViewport(const sf::RenderWindow& win)
{
    // TODO: por si cambia de tamaño
}

void HUD::update(float /*dt*/)
{
    const std::string timeStr = formatTimeMMSS(RunStats::elapsed());
    const int deaths = RunStats::deaths();
    m_text.setString("Tiempo " + timeStr + "   Muertes " + std::to_string(deaths));
}


void HUD::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(m_text, states);
}

