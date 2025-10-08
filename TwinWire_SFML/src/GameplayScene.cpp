#include "GameplayScene.h"
#include "Game.h"
#include "SpriteAnimator.h"   // para FrameMeta
#include "Pillar.h"
#include "IChockeable.h"
#include <utility>
#include <cmath>
#include <iostream>
#include <optional>
#include "BulletEmitter.h"
#include "Orb.h"
#include "PivotDataIO_CSV.h"
#include "WinScene.h"
#include "SFML/Graphics.hpp"



#ifdef _DEBUG
static void drawCross(sf::RenderTarget& tgt, sf::Vector2f P, sf::Color c = sf::Color::Red) {
    sf::Vertex h[2]{ {P+sf::Vector2f{-4,0}, c}, {P+sf::Vector2f{4,0}, c} };
    sf::Vertex v[2]{ {P+sf::Vector2f{0,-4}, c}, {P+sf::Vector2f{0,4}, c} };
    tgt.draw(h, 2, sf::PrimitiveType::Lines);
    tgt.draw(v, 2, sf::PrimitiveType::Lines);
}
#endif



// Helpers para construir el AABB del player (mundo)
static sf::FloatRect makePlayerAABB(sf::Vector2f playerPos) {
    const sf::Vector2f size   {10.f, 28.f};   
    const sf::Vector2f offset {5.f, -25.f};    
    const sf::Vector2f origin = playerPos + offset - size * 0.5f;
    return sf::FloatRect(origin, size);
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

// Version boolean con outHit 
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
            const SegClip c = segmentVsRect(a, b, p->bounds());
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

void GameplayScene::PauseSetUp(Game& game)
{
    m_uiFont = m_res.getFont("../../../../res/Assets/Fonts/PixelifySans-VariableFont_wght.ttf");
    m_pause.build(*m_uiFont, game.Window().getSize());

    m_pause.setOnResume([this]()
    {
        m_pause.hide();
    });

    m_pause.setOnOptions([]()
    {
       // TODO: abrir submenu de opciones cuando este listo 
    });

    m_pause.setOnExit([&game]()
    {
       game.SwitchTo(SceneId::MainMenu); 
    });
}

void GameplayScene::onEnter(Game& game)
{
    resetAll();
    m_playerDead = false;
    m_restarTimer = -1.f;
    
    
    // Carga pivots desde archivo si ya existe
    PivotDataIO_CSV::load(
        m_player.animator(),
        "../../../../res/Assets/Pivots/pivots.csv"
    );

    
    // Fondo
    m_bgTex = m_res.getTexture("../Assets/Backgrounds/BackGround.png");
    if (m_bgTex) {
        m_bgTex->setSmooth(false);
        m_bgSprite = std::make_unique<sf::Sprite>(*m_bgTex);
        rebuildBackgroundForWindow(game.Window());
    }

    const std::string kBulletTex = "../Assets/Sprites/BossBullet1.png";
    const sf::Vector2f kBulletScale{0.35f, 0.35f};

    m_emitter = std::make_unique<BulletEmitter>(
        m_bullets, m_res, kBulletTex, kBulletScale
    );
    
    const sf::Vector2u win = game.Window().getSize();
    const float w = static_cast<float>(win.x);
    const float h = static_cast<float>(win.y);
    const float margin = 192.f; // margen fuera de la camara
    
    const float groundY = win.y * 0.965f; 

    
    
    m_cullRect = sf::FloatRect({-margin, -margin}, {w + margin * 2.f, h + margin * 2.f});
    

    Pillar& p1Left = spawnPillar(m_res, "../Assets/Sprites/Pillar.png",
                             std::nullopt,
                             {win.x * 0.05f, groundY}, 1.f);

    Pillar& p2Right = spawnPillar(m_res, "../Assets/Sprites/Pillar.png",
                             std::nullopt,
                             {win.x * 0.95f, groundY}, 1.f);

    // altura de los orbes
    const float yOrbs = win.y * 0.60f;

    Orb& oLeft = spawnOrb(m_res, "../Assets/Sprites/Orb.png", {win.x * 0.2f, yOrbs}, 1.f);
    Orb& oRight = spawnOrb(m_res, "../Assets/Sprites/Orb.png", {win.x * 0.8f, yOrbs}, 1.f);

    m_orbViews.clear();
    m_orbViews.reserve(m_orbs.size());
    for (std::unique_ptr<Orb>& up : m_orbs)
    {
        if (up)
        {
            m_orbViews.push_back(static_cast<IOrb*>(up.get())); // Orb* -> IOrb*
        }
    }

    // Intancio al boss
    
    Boss::Params bp;
    const float W = static_cast<float>(win.x);
    const float H = static_cast<float>(win.y);

    bp.patrolBounds = sf::FloatRect(
        sf::Vector2f(W * 0.1f, 0.f),  
        sf::Vector2f(W * 0.8f, 1.f)    
    );
    
    bp.startPos= { W * 0.5f, H * 0.25f };
    
    bp.attackBlockMinP1 = 3.0f; bp.attackBlockMaxP1 = 5.0f;
    bp.attackBlockMinP2 = 5.0f; bp.attackBlockMaxP2 = 7.0f;

    bp.waveDurationMin = 0.6f;  bp.waveDurationMax = 1.2f;
    bp.waveGapMin = 0.25f; bp.waveGapMax      = 0.6f;

    // densidad de spokes
    bp.ringMin = 24; bp.ringMax = 28;
    bp.ringSpeedP1 = 400.f; bp.ringSpeedP2 = 600.f;

    bp.toPlayerSpeedP1 = 500.f; bp.toPlayerSpeedP2 = 1000.f;
    bp.toPlayerPulseMin = 0.18f; bp.toPlayerPulseMax = 0.28f;

    // velocidad de las balas
    bp.ringSpeedP2 = 350.f; 
    
    // intervalo entre anillos (lineas de puntos mas juntas o separadas)
    bp.ringIntervalMin = 0.15f; bp.ringIntervalMax = 0.25f;
    bp.gapWidthMinDeg = 35.f;  bp.gapWidthMaxDeg  = 45.f;

    // cuanto se mueve el pasillo por ring (lento = mas jugable)
    bp.gapStepMinDeg = 2.f;
    bp.gapStepMaxDeg = 4.f;
    
    // spokes fijos 
    bp.spinPerRingMinDeg = 0.f;
    bp.spinPerRingMaxDeg = 0.f; 

    m_boss = std::make_unique<Boss>(bp, m_emitter.get(), &m_orbViews);

    m_boss->setOnDeath([this, &game]() {
     // Deferimos la reacción a fin de frame (evitar destruir cosas en medio del update del boss)
     m_deferred.push_back([this, &game]() {

         game.SwitchTo(SceneId::WinScene);
         m_boss.reset();
        });
    });

    Player* playerRaw = &m_player;
    
    m_boss->setPlayerPosProvider([playerRaw]() -> sf::Vector2f {
     return playerRaw ? playerRaw->getPosition() /*o center()*/ : sf::Vector2f{};
    });
    
    p1Left.setTargetOrb(&oLeft);
    p2Right.setTargetOrb(&oRight);
    
    p1Left.setFallDelay(0.9f);
    p2Right.setFallDelay(0.9f);

    // pilar respawn
    p1Left.respawnIn(0.5f);
    p2Right.respawnIn(0.5f);
    
    m_query = std::make_unique<MultiPillarQuery>(); // ctor vacío
    m_query->pillars = &m_livePtrs;                
    m_player.setChokeQuery(m_query.get());
    
    m_player.setChokeQuery(m_query.get());
    

    m_player.setVisualScale(2.2f);

   
    //Setea SOLO el primer frame (queda fijo por ahora)
    //m_player.setFrames({ { sf::IntRect{{0,0},{18,32}}, 9.f, 999.f } }, true);
    // aplica pivots ajustados desde archivo (si existe)
    PivotDataIO_CSV::load(m_player.animator(), "../Assets/Pivots/pivots.csv");

    //Posicion inicial comoda
    m_player.setPosition({ win.x * 0.2f, win.y * 0.965f});
    m_player.play(AnimId::Idle, true);
    
    m_restarTimer = -1.f;

    PauseSetUp(game);
    m_pause.hide();
}

void GameplayScene::onExit(Game& game)
{
    m_playerDead = false;
   // m_player.reset();
}

#ifdef _DEBUG

static void fireOneTowardMouse(BulletEmitter& emitter, const sf::RenderWindow& win, sf::Vector2f origin) {
    const sf::Vector2i pix   = sf::Mouse::getPosition(win);
    const sf::Vector2f world = win.mapPixelToCoords(pix);
    const sf::Vector2f dir   = world - origin;
    const float angle = std::atan2(dir.y, dir.x);
    emitter.emit(origin, angle, /*speed*/ 600.f);
}

static void fireRing(BulletEmitter& e, sf::Vector2f o, int n, float spd){
    const float twoPi=6.28318530718f, step=twoPi/static_cast<float>(n);
    for (int i=0;i<n;++i) e.emit(o, i*step, spd);
}


#endif



void GameplayScene::handleEvent(Game& game, const sf::Event& ev)
{
    if (ev.is<sf::Event::Closed>())
        game.RequestQuit();

    if (const sf::Event::Resized* r = ev.getIf<sf::Event::Resized>()) {
        const float w = static_cast<float>(r->size.x);
        const float h = static_cast<float>(r->size.y);
        const float margin = 192.f;
        m_cullRect = sf::FloatRect({-margin, -margin}, {w + margin * 2.f, h + margin * 2.f});
        m_pause.build(*m_uiFont, { r->size.x, r->size.y });
    }

    if (ev.is<sf::Event::KeyPressed>()) {
        const auto& kp = *ev.getIf<sf::Event::KeyPressed>();
        if (kp.code == sf::Keyboard::Key::Escape) {
            if (!m_uiFont) PauseSetUp(game); 
            m_pause.toggle();
            return; // el return previene las acciones en el frame
        }
    }
    // Si la pausa es visible, manda un evento a "pauselayer" y para
    if (m_pause.isVisible()) {
        m_pause.handleEvent(ev, game.Window());
        return;
    }

#ifdef _DEBUG
    if (ev.is<sf::Event::KeyPressed>()) {
        const sf::Event::KeyPressed* kp = ev.getIf<sf::Event::KeyPressed>();
        if (kp->code == sf::Keyboard::Key::J && m_emitter) {
            // origen: se usa player.getPosition()
            const auto win = game.Window().getSize();
            sf::Vector2f origin{ static_cast<float>(win.x) * 0.5f,
                                 static_cast<float>(win.y) * 0.5f };
            fireRing(*m_emitter, origin, 10, 600.f);
        }
    }

    if (ev.is<sf::Event::KeyPressed>()) {
        const auto& kp = *ev.getIf<sf::Event::KeyPressed>();

        if (kp.code == sf::Keyboard::Key::F10) {
            m_pivotTunerActive = !m_pivotTunerActive;
        }

        if (m_pivotTunerActive) {
            const bool fine = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)
                           || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift);
            const float step = fine ? 0.25f : 1.f;

            if (kp.code == sf::Keyboard::Key::Left)  m_player.animator().nudgePivotX(-step);
            if (kp.code == sf::Keyboard::Key::Right) m_player.animator().nudgePivotX(+step);

            if (kp.code == sf::Keyboard::Key::F12) {
                const int count = m_player.animator().frameCount();
                std::cerr << "[PivotIO] F12 pressed  frames=" << count << "\n";
                for (int i = 0; i < count; ++i) {
                    const auto& fr = m_player.animator().frames()[i];
                    std::cerr << "  i="<<i<<" rect=("
                              << fr.rect.position.x << "," << fr.rect.position.y << ","
                              << fr.rect.size.x << "," << fr.rect.size.y << ") pivot="
                              << fr.pivotX << " dur=" << fr.duration << "\n";
                }
                const bool ok = PivotDataIO_CSV::save(
                    m_player.animator(), "../../../../res/Assets/Pivots/pivots.csv"
                );
                std::cerr << "[PivotIO] save " << (ok ? "OK" : "FAIL") << "\n";
            }
        }
    }

#endif
    
}


void GameplayScene::handleInput(Game& game)
{
    if (m_pause.isVisible()) return;
    if (m_playerDead) return; // saco los controles si muere
   m_player.handleInput(); // A/D 
}

void GameplayScene::update(Game& game, float dt)
{
    if (dt > 0.05f) dt = 0.05f;
    
    if (m_pause.isVisible()) {
        m_pause.update(dt);
        return; // freezea el juego mientras esta en pausa
    }
    
    const sf::Vector2f p = m_player.getPosition();    
    m_playerAABB = makePlayerAABB(p);

    bool playerHit = false;
    m_bullets.update(dt, m_cullRect, m_playerAABB, playerHit);

    const sf::Vector2f feet = m_player.getFeetWorld();
    const sf::Vector2f hbSize   {16.f, 22.f};  // aca se ajusta
    const sf::Vector2f hbOffset { 0.f, -6.f};  // sube/baja/centra desde los pies
    const sf::Vector2f hbOrigin { feet.x - hbSize.x*0.5f + hbOffset.x,
                                  feet.y - hbSize.y       + hbOffset.y };
    m_playerAABB = sf::FloatRect(hbOrigin, hbSize);
    
    // Pilar Update
    for (std::unique_ptr<Pillar>& pillar : m_pillars) if (pillar) pillar->update(dt);

    // Orbe Update
    for (std::unique_ptr<Orb>& o : m_orbs)
        if (o) o->update(dt);
    
   
    if (m_boss) m_boss->update(dt);

    if (m_playerDead)
    {
        m_player.animator().update(dt);
        m_restarTimer -= dt;
        if (m_restarTimer <= 0.f)
        {
            game.reloadCurrentScene(); // <- reinicia ( On enter hace reset)
        }
        return;
    }
    
    if (playerHit) {
        // entra al estado de muerte
        m_playerDead = true;

        //Reproduce Die una sola vez y congela al final
        m_player.play(AnimId::Die, false, true);
        
        m_restarTimer = 1.0f;

        return;
    }
    
    m_player.update(dt, game.Window()); //flip hacia le mouse
   
    // No es estar checkeando, solo ejecuta si hay tarea pendiente (cuando el boss llama al callback)
    for (std::function<void()>& fn : m_deferred) fn();
    m_deferred.clear();
}

void GameplayScene::draw(Game& game, sf::RenderTarget& target)
{
    
    
    if (m_bgSprite) target.draw(*m_bgSprite);

    // ---- Orbs ----
    for (std::unique_ptr<Orb>& orb : m_orbs)
    {
        if (!orb) continue;
        if (!orb->isActive()) continue; 

        // sprite del orbe
        orb->draw(target);

#ifdef _DEBUG

        if (m_pivotTunerActive) {
            const sf::Vector2f feet = m_player.animator().sprite().getPosition(); // origin = pivotX/base
            drawCross(target, feet);
        }
        
#endif
        
        
    }
    
    
    for (std::unique_ptr<Pillar>& uniquePillar : m_pillars)
    {
        if (!uniquePillar || !uniquePillar->isActive()) continue;

        // Dibujo normal del pilar
        uniquePillar->draw(target);

        // Debug AABB ( FloatRect.position / .size, 3 horas con esto me mato)
        const auto r = uniquePillar->bounds();
        sf::RectangleShape box({ r.size.x, r.size.y });
        box.setPosition(r.position);
        box.setFillColor(sf::Color::Transparent);
        box.setOutlineThickness(1.f);
        box.setOutlineColor(sf::Color::Red);
        target.draw(box);
    }

    m_bullets.draw(target);

    m_player.draw(target);

    if (m_boss) target.draw(*m_boss);

    // al final para que se dibuje arriba de todo
    if (m_pause.isVisible())
        m_pause.draw(target);
        
#ifdef _DEBUG
    sf::RectangleShape r;
    r.setSize(m_playerAABB.size);
    r.setPosition(m_playerAABB.position);
    r.setFillColor(sf::Color::Transparent);
    r.setOutlineThickness(1.f);
    r.setOutlineColor(sf::Color::Green);
    target.draw(r);
#endif
}

void GameplayScene::rebuildBackgroundForWindow(const sf::RenderWindow& window)
{
    if (!m_bgTex || !m_bgSprite) return;

    const sf::Vector2u tex = m_bgTex->getSize();
    const sf::Vector2u win = window.getSize();

    const float sx = static_cast<float>(win.x) / tex.x;
    const float sy = static_cast<float>(win.y) / tex.y;

    float scale = 1.f;

    if (m_bgPixelPerfect) {
        //  Pixel perfect 
        const bool needUp  = (sx >= 1.f && sy >= 1.f);
        const bool needDown= (sx <  1.f ||  sy <  1.f);

        if (needUp) {
            // Upscale entero (2x, 3x)
            const float k = std::floor(std::min(sx, sy));
            scale = std::max(1.f, k);
        } else if (needDown) {
            // Downscale entero (1/2, 1/3)
            const float kx = std::ceil(static_cast<float>(tex.x) / win.x);
            const float ky = std::ceil(static_cast<float>(tex.y) / win.y);
            const float k  = std::max(kx, ky);     // divisor entero
            scale = 1.f / k;
        }
    } else {
        // --- Fit dentro de la ventana ---
        scale = std::min(sx, sy);
    }

    m_bgSprite->setScale({ scale, scale });

    // Centrar
    const sf::Vector2f bgSize{ tex.x * scale, tex.y * scale };
    const sf::Vector2f pos{ (win.x - bgSize.x) * 0.5f, (win.y - bgSize.y) * 0.5f };
    m_bgSprite->setPosition(pos);
    
}

Orb& GameplayScene::spawnOrb(ResouceManager& rm, const std::string& tex, sf::Vector2f pos, float scale)
{
    std::unique_ptr<Orb>& up = m_orbs.emplace_back(std::make_unique<Orb>(rm, tex, pos, scale));
    Orb* raw = up.get();

    return *raw;
}

Pillar& GameplayScene::spawnPillar(ResouceManager& rm, const std::string& tex,
                                   std::optional<sf::IntRect> rect, sf::Vector2f pos, float scale)
{
    std::unique_ptr<Pillar>& up = m_pillars.emplace_back(std::make_unique<Pillar>(rm, tex, rect, pos, scale));
    m_livePtrs.push_back(up.get());
    return *up;
}

void GameplayScene::resetAll()
{
    m_playerDead = false;
    m_restarTimer = -1.f;

    // Restauro al player (pos, vel, animacion idle, flags, filamentos)
    //m_player.setPosition()
    m_player.play(AnimId::Idle, true, false);

    m_livePtrs.clear();
    m_orbViews.clear();

    // Reset bullets/orbes/pilares
    m_bullets.clear();
    for (std::unique_ptr<Orb>& o : m_orbs) o.reset();
    for (std::unique_ptr<Pillar>& p : m_pillars) p.reset();
    
}




