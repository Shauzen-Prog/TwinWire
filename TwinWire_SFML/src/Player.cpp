#include "Player.h"

Player::Player(ResouceManager& rm, const std::string& sheetPath)
: m_rm(rm)
, m_sheet(m_rm.getTexture(sheetPath)) // obtengo la textura
, m_sprite(*m_sheet) // contruyo el sprite con la textura
, m_anim(m_sprite) // y el animator con el sprite
{
    m_sheet = m_rm.getTexture(sheetPath);
    m_sheet->setSmooth(true);
    m_sprite.setPosition(m_pos);
}

void Player::setFrames(const std::vector<FrameMeta>& frames, bool loop)
{
    // El animator aplica rect + pivotX por frame y deja Y en base del rect
    m_anim.setTexture(*m_sheet);
    m_anim.setFrames(frames, loop);
    // Nota: el origen del sprite lo fija el animator en applyCurrentFrame()
}

void Player::handleInput()
{
    float x = 0.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) x += 1.f;
    m_vel.x = x * m_speed;
}

void Player::update(float dt, const sf::RenderWindow& window)
{
    // Mover
    m_pos += m_vel * dt;
    m_sprite.setPosition(m_pos);

    // Mirar hacia el mouse (flip horizontal alrededor del pivot)
    const sf::Vector2i mp = sf::Mouse::getPosition(window);
    m_facingRight = (static_cast<float>(mp.x) >= m_pos.x);
    const float sx = m_facingRight ? 1.f : -1.f;
    m_sprite.setScale({ sx * m_visualScale, m_visualScale });

    // Animación
    m_anim.update(dt);
}

void Player::draw(sf::RenderTarget& target) const
{
    target.draw(m_sprite);
}

void Player::setPosition(sf::Vector2f p)
{
    m_pos = p;
    m_sprite.setPosition(m_pos);
}

sf::Vector2f Player::getPosition() const
{
    return m_pos;
}

void Player::setHandSocketLocal(sf::Vector2f local)
{
    m_handSocketLocal = local;
}

sf::Vector2f Player::getHandSocketWorld() const
{
    // Usa el transform completo del sprite (posición, escala/flip, origen por frame)
    return m_sprite.getTransform().transformPoint(m_handSocketLocal);
}

sf::Vector2f Player::computeShootDirToMouse(const sf::RenderWindow& window) const
{
    const sf::Vector2f origin = getHandSocketWorld();
    const sf::Vector2i mp     = sf::Mouse::getPosition(window);
    const sf::Vector2f target { static_cast<float>(mp.x), static_cast<float>(mp.y) };

    sf::Vector2f d = target - origin;
    const float len2 = d.x*d.x + d.y*d.y;
    if (len2 <= 1e-6f) return { 1.f, 0.f }; // fallback
    const float inv = 1.f / std::sqrt(len2);
    return d * inv;
}

void Player::setVisualScale(float s) {
    m_visualScale = s;
    // aplica inmediatamente respetando el flip
    const float sx = m_facingRight ? 1.f : -1.f;
    m_sprite.setScale({ sx * m_visualScale, m_visualScale });
}

