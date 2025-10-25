#include "PlayerTunerScene.h"
#include "Game.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include "PlayerAABB.h"

// <summary>Escena mínima para ajustar la hitbox del Player y chequear animaciones.</summary>
void PlayerTunerScene::rebuildOverlay()
{
    if (!m_fontOk) return;

    const auto R = m_player.handSocketLocal(Player::Hand::Right);
    const auto L = m_player.handSocketLocal(Player::Hand::Left);

    std::ostringstream oss;
    oss.setf(std::ios::fixed); oss << std::setprecision(2);
    oss << "[Player Tuner - Filaments]\n"
        << "Active Hand: " << (m_activeHand == Player::Hand::Right ? "Right (F6)" : "Left (F6)") << "\n"
        << "Right Socket: (" << R.x << ", " << R.y << ")\n"
        << "Left  Socket: (" << L.x << ", " << L.y << ")\n"
        << "Move active socket:  ,  .  ;  '\n"
        << "Dump sockets: F10\n";
    m_text.setString(oss.str());
    m_text.setPosition({12.f, 10.f});
}



PlayerTunerScene::PlayerTunerScene(std::string sheetPath)
    : m_sheetPath(std::move(sheetPath))
      , m_player(m_res, m_sheetPath), m_text(*m_font)
{
}

void PlayerTunerScene::onEnter(Game& game)
{
    // Posicionamos al player cerca del "suelo" de la ventana.
    const auto winSize = game.Window().getSize();
    m_player.setPosition({ winSize.x * 0.5f, winSize.y * 0.965f });
    // Aseguramos una animación neutra (no modifica nada más).
    // Si ya la establecés en Player, podés omitirlo.
    // m_player.play(AnimId::Idle, true);

    // Fuente opcional para overlay
    try {
        m_font = m_res.getFont("../Assets/Font/DejaVuSans.ttf");
        if (m_font) {
            m_text.setFont(*m_font);
            m_text.setCharacterSize(16);
            m_text.setFillColor(sf::Color(200, 220, 255));
            m_fontOk = true;
        }
    } catch(...) {
        m_fontOk = false;
    }
    rebuildOverlay();
}

void PlayerTunerScene::onExit(Game& /*game*/)
{
    // Nada especial
}

void PlayerTunerScene::handleEvent(Game& game, const sf::Event& ev)
{
    if (ev.is<sf::Event::Closed>()) game.RequestQuit();

    if (const auto* r = ev.getIf<sf::Event::Resized>()) {
        // Reposicionar sobre “suelo” al redimensionar
        m_player.setPosition({ r->size.x * 0.5f, r->size.y * 0.965f });
    }

    if (ev.is<sf::Event::KeyPressed>()) {
        const auto& kp = *ev.getIf<sf::Event::KeyPressed>();
        switch (kp.code) {
            // Cambiar mano activa
        case sf::Keyboard::Key::F6:
            m_activeHand = (m_activeHand == Player::Hand::Right)
                         ? Player::Hand::Left
                         : Player::Hand::Right;
            rebuildOverlay();
            break;

            // Ajuste fino del socket (mano activa)
        case sf::Keyboard::Key::Comma:      // ','
            m_player.adjustHandSocket(m_activeHand, { -1.f,  0.f }); rebuildOverlay(); break;
        case sf::Keyboard::Key::Period:     // '.'
            m_player.adjustHandSocket(m_activeHand, { +1.f,  0.f }); rebuildOverlay(); break;
        case sf::Keyboard::Key::Semicolon:  // ';'
            m_player.adjustHandSocket(m_activeHand, {  0.f, -1.f }); rebuildOverlay(); break;
        case sf::Keyboard::Key::Apostrophe: // '''
            m_player.adjustHandSocket(m_activeHand, {  0.f, +1.f }); rebuildOverlay(); break;

            // Dump de sockets para pegar en Player::Tuning
        case sf::Keyboard::Key::F10: {
                const auto R = m_player.handSocketLocal(Player::Hand::Right);
                const auto L = m_player.handSocketLocal(Player::Hand::Left);
                std::cout << "\n// --- Player::Tuning sockets ---\n"
                          << "socketRightLocal = {" << R.x << "f, " << R.y << "f};\n"
                          << "socketLeftLocal  = {" << L.x << "f, " << L.y << "f};\n";
                break;
        }
        default: break;
        }
    }
}

void PlayerTunerScene::handleInput(Game& /*game*/)
{
    // Reusar el input del Player (A/D)
    m_player.handleInput();
}

void PlayerTunerScene::update(Game& game, float dt)
{
    if (dt > 0.05f) dt = 0.05f;

    auto feet = m_player.getFeetWorld();
    m_player.update(dt, game.Window());
}

void PlayerTunerScene::draw(Game& game, sf::RenderTarget& rt)
{
    rt.clear(sf::Color(20, 18, 24));

    // Dibujar Player
    m_player.draw(rt);

    // Debug visual: cruz en sockets (derecha = cian, izquierda = magenta)
    const auto WR = m_player.handSocketWorld(Player::Hand::Right);
    const auto WL = m_player.handSocketWorld(Player::Hand::Left);

#ifdef _DEBUG
    drawCross(rt, WR, sf::Color(0, 255, 255));
    drawCross(rt, WL, sf::Color(255, 0, 255));
#endif

    // Overlay
    if (m_fontOk) rt.draw(m_text);
}


void PlayerTunerScene::resetDefaults()
{
    m_visualScale = 2.0f;
    m_player.setVisualScale(m_visualScale);
    m_hbSize   = {16.f, 22.f};
    m_hbOffset = { 0.f,-6.f};
    rebuildOverlay();
}

#ifdef _DEBUG
void PlayerTunerScene::drawCross(sf::RenderTarget& tgt, sf::Vector2f P, sf::Color c)
{
    sf::Vertex h[2]{ {P + sf::Vector2f{-4.f, 0.f}, c}, {P + sf::Vector2f{4.f, 0.f}, c} };
    sf::Vertex v[2]{ {P + sf::Vector2f{0.f, -4.f}, c}, {P + sf::Vector2f{0.f, 4.f}, c} };
    tgt.draw(h, 2, sf::PrimitiveType::Lines);
    tgt.draw(v, 2, sf::PrimitiveType::Lines);
}
#endif