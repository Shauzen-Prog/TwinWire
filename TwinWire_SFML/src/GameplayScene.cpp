#include "GameplayScene.h"
#include "Game.h"
#include "SpriteAnimator.h"   // para FrameMeta
#include "Pillar.h"
#include "IChockeable.h"
#include <utility>
#include <cmath>

// Frame único: 0,0 → 18,31 (width=19, height=32), pivotX ≈ 9 px.
// duration grande para “no avanzar” (queda fijo).
static std::vector<FrameMeta> makeInitialIdleFrames()
{
    return {
        FrameMeta{ sf::IntRect{ {0, 0}, {19, 32} }, 9.f, 999.f }
    };
}


// ----- Helpers de recorte segmento vs AABB  -----
struct SegClip {
    bool hit{ false };
    float t{ 0.f };             // parámetro de entrada [0..1]
    sf::Vector2f p{};         // punto de impacto
};

static inline SegClip segmentVsRect(const sf::Vector2f& a,
    const sf::Vector2f& b,
    const sf::FloatRect& r)
{
    const float L = r.position.x;
    const float T = r.position.y;
    const float R = L + r.size.x;
    const float B = T + r.size.y;

    const sf::Vector2f d = b - a;
    float t0 = 0.f, t1 = 1.f;

    auto clip = [&](float p, float q)->bool {
        if (std::abs(p) < 1e-6f) return q >= 0.f;   // paralelo: adentro si q>=0
        const float t = q / p;
        if (p < 0.f) { if (t > t1) return false; if (t > t0) t0 = t; }
        else { if (t < t0) return false; if (t < t1) t1 = t; }
        return true;
        };

    if (!clip(-d.x, a.x - L)) return {};
    if (!clip(d.x, R - a.x)) return {};
    if (!clip(-d.y, a.y - T)) return {};
    if (!clip(d.y, B - a.y)) return {};

    return { true, t0, { a.x + d.x * t0, a.y + d.y * t0 } };
}

// Versión boolean con outHit 
static inline bool segmentVsRect(const sf::Vector2f& a,
    const sf::Vector2f& b,
    const sf::FloatRect& r,
    sf::Vector2f& outHit)
{
    const SegClip c = segmentVsRect(a, b, r);
    if (!c.hit) return false;
    outHit = c.p;
    return true;
}


struct GameplayScene::MultiPillarQuery : IChokeQuery {
    std::vector<Pillar*>* pillars = nullptr;

    IChockeable* RaycastChoke(const sf::Vector2f& a,
        const sf::Vector2f& b,
        sf::Vector2f& outHit) override
    {
        if (!pillars) return nullptr;
        IChockeable* best = nullptr; float bestT = 1e9f; sf::Vector2f bestP{};
        for (Pillar* p : *pillars) {
            if (!p || !p->isActive()) continue;
            const auto c = segmentVsRect(a, b, p->bounds());
            if (c.hit && c.t < bestT) { bestT = c.t; bestP = c.p; best = p; }
        }
        if (best) outHit = bestP;
        return best;
    }
};




GameplayScene::GameplayScene(std::string sheetPath)
: m_sheetPath(std::move(sheetPath)), m_player(m_res, "../Assets/Sprites/Player/PlayerSpriteSheet.png")
{
    
}


void GameplayScene::onEnter(Game& game)
{

    // Fondo
    m_bgTex = m_res.getTexture("../Assets/Backgrounds/BackGround.png"); // a
    if (m_bgTex) {
        m_bgTex->setSmooth(false);
        m_bgSprite = std::make_unique<sf::Sprite>(*m_bgTex);
        rebuildBackgroundForWindow(game.Window());
    }

    const auto win = game.Window().getSize();
    const float groundY = win.y * 0.965f;

    spawnPillar(m_res, "../Assets/Sprites/Pillar.png",
        std::nullopt,
        { win.x * 0.55f, groundY }, 1.f);

    spawnPillar(m_res, "../Assets/Sprites/Pillar.png",
        std::nullopt,
        { win.x * 0.70f, groundY }, 1.f);


    m_query = std::make_unique<MultiPillarQuery>(); // ctor vacío
    m_query->pillars = &m_livePtrs;                
    m_player.setChokeQuery(m_query.get());
    
    m_player.setChokeQuery(m_query.get());
    

    m_player.setVisualScale(2.2f);
    
    //Setea SOLO el primer frame (queda fijo por ahora)
    m_player.setFrames({ { sf::IntRect{{0,0},{18,32}}, 9.f, 999.f } }, true);

    //Posicion inicial comoda
 
    m_player.setPosition({ win.x * 0.2f, win.y * 0.965f });
}

void GameplayScene::onExit(Game& game)
{
   // m_player.reset();
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
   m_player.handleInput(); // A/D 
}

void GameplayScene::update(Game& game, float dt)
{
    for (auto& p : m_pillars) if (p) p->update(dt);
    m_player.update(dt, game.Window()); //flip hacia le mouse
}

void GameplayScene::draw(Game& game, sf::RenderTarget& target)
{
 
    if (m_bgSprite) target.draw(*m_bgSprite);

    for (auto& up : m_pillars)
    {
        if (!up || !up->isActive()) continue;

        // Dibujo normal del pilar
        up->draw(target);

        // Debug AABB (SFML 3: FloatRect.position / .size, 3 horas con esto me mato)
        const auto r = up->bounds();
        sf::RectangleShape box({ r.size.x, r.size.y });
        box.setPosition(r.position);
        box.setFillColor(sf::Color::Transparent);
        box.setOutlineThickness(1.f);
        box.setOutlineColor(sf::Color::Red);
        target.draw(box);
    }

    m_player.draw(target);
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

Pillar& GameplayScene::spawnPillar(ResouceManager& rm, const std::string& tex,
                                   std::optional<sf::IntRect> rect, sf::Vector2f pos, float scale)
{
    std::unique_ptr<Pillar>& up = m_pillars.emplace_back(std::make_unique<Pillar>(rm, tex, rect, pos, scale));
    m_livePtrs.push_back(up.get());
    return *up;
}



