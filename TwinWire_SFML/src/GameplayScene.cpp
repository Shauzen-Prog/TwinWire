#include "GameplayScene.h"
#include "Game.h"
#include "SpriteAnimator.h"   // para FrameMeta
#include <utility>

// Frame único: 0,0 → 18,31 (width=19, height=32), pivotX ≈ 9 px.
// duration grande para “no avanzar” (queda fijo).
static std::vector<FrameMeta> makeInitialIdleFrames()
{
    return {
        FrameMeta{ sf::IntRect{ {0, 0}, {19, 32} }, 9.f, 999.f }
    };
}

GameplayScene::GameplayScene(std::string sheetPath)
: m_sheetPath(std::move(sheetPath))
{

}

void GameplayScene::onEnter(Game& game)
{

    // Fondo
    m_bgTex = m_res.getTexture("../Assets/Backgrounds/Bg.png");
    if (m_bgTex) {
        m_bgTex->setSmooth(false);
        m_bgSprite = std::make_unique<sf::Sprite>(*m_bgTex);
        rebuildBackgroundForWindow(game.Window());
    }
    
    //construyo al player
    m_player = std::make_unique<Player>(m_res, m_sheetPath);

    m_player->setVisualScale(2.f);
    
    //Setea SOLO el primer frame (queda fijo por ahora)
    m_player->setFrames({ { sf::IntRect{{0,0},{18,32}}, 9.f, 999.f } }, true);

    //Posicion inicial comoda
    const sf::Vector2u win = game.Window().getSize();
    m_player->setPosition({ win.x * 0.5f, win.y * 0.5f });
}

void GameplayScene::onExit(Game& game)
{
    m_player.reset();
}

void GameplayScene::handleEvent(Game& game, const sf::Event& ev)
{
    if (ev.is<sf::Event::Closed>())
    {
        game.RequestQuit();
    }
    // mas adelante clicks para filamentos, etc
}

void GameplayScene::handleInput(Game& game)
{
    if (m_player) m_player->handleInput(); // A/D 
}

void GameplayScene::update(Game& game, float dt)
{
    if (m_player) m_player->update(dt, game.Window()); //flip hacia le mouse
}

void GameplayScene::draw(Game& game, sf::RenderTarget& target)
{
    if (m_bgSprite) target.draw(*m_bgSprite);
    if (m_player)   m_player->draw(target);
}

void GameplayScene::rebuildBackgroundForWindow(const sf::RenderWindow& window)
{
    if (!m_bgTex || !m_bgSprite) return;

    const auto tex = m_bgTex->getSize();
    const auto win = window.getSize();

    const float sx = static_cast<float>(win.x) / tex.x;
    const float sy = static_cast<float>(win.y) / tex.y;

    float scale = 1.f;

    if (m_bgPixelPerfect) {
        // --- Pixel-perfect (factores enteros) ---
        const bool needUp  = (sx >= 1.f && sy >= 1.f);
        const bool needDown= (sx <  1.f ||  sy <  1.f);

        if (needUp) {
            // Upscale entero (2x, 3x, …)
            const float k = std::floor(std::min(sx, sy));
            scale = std::max(1.f, k);
        } else if (needDown) {
            // Downscale entero (1/2, 1/3, …)
            const float kx = std::ceil(static_cast<float>(tex.x) / win.x);
            const float ky = std::ceil(static_cast<float>(tex.y) / win.y);
            const float k  = std::max(kx, ky);     // divisor entero
            scale = 1.f / k;
        }
    } else {
        // --- Fit dentro de la ventana (uniforme, puede no ser entero) ---
        scale = std::min(sx, sy);
    }

    m_bgSprite->setScale({ scale, scale });

    // Centrar
    const sf::Vector2f bgSize{ tex.x * scale, tex.y * scale };
    const sf::Vector2f pos{ (win.x - bgSize.x) * 0.5f, (win.y - bgSize.y) * 0.5f };
    m_bgSprite->setPosition(pos);
    
}

