#include "MainMenuScene.h"
#include <iostream>
#include "Game.h"
#include <cstdio>
#include <sstream>

#include "RunStats.h"
#include "HighScore.h"
#include "linq.h"
#include "Boss.h"
#include "BulletEmitter.h"

// --- UI SFX (press only) ---
static const char* SFX_PRESS = "res/Assets/Audio/SFX/ButtonPress.wav";
struct BtnRect { sf::FloatRect r{}; };
static BtnRect s_play, s_play2,s_options, s_credits, s_exit;

static inline sf::FloatRect makeRectFromCenter(sf::Vector2f c, sf::Vector2f size) {
    return { c - size * 0.5f, size };
}

static std::string fmtMMSS(float t)
{
    int total = static_cast<int>(t + 0.5f);
    int m = total / 60, s = total % 60;
    char buf[16]; std::snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
    return std::string(buf);
}

MainMenuScene::MainMenuScene()
: m_font(m_rm.getFont("res/Assets/Fonts/PixelifySans-VariableFont_wght.ttf")),
  m_title(*m_font, "", 13),
  m_best{*m_font, "", 20u},
  m_creditsTitle(*m_font, "Creditos", 36u),
  m_creditsBody(*m_font, "", 22u),
  m_howToTitle(*m_font, "Como jugar"),
  m_howToBody(*m_font,"",20u)
{
    // Creo la semilla del random
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    m_best = sf::Text(*m_font, "", 25u);
    m_best.setFillColor(sf::Color::White);
    m_best.setOutlineColor(sf::Color::Black);
    m_best.setOutlineThickness(3.f);
    m_best.setPosition({10.f, 10.f});

    m_scoresTop.emplace(*m_font, "", 20u);
    m_scoresTop->setFillColor(sf::Color::White);
    m_scoresTop->setOutlineColor(sf::Color::Black);
    m_scoresTop->setOutlineThickness(2.f);
    m_scoresTop->setPosition({10.f, 10.f + 28.f});
    
    m_creditsTitle.setFillColor(sf::Color(230,230,230));
    m_creditsTitle.setOutlineColor(sf::Color(0,0,0,140));
    m_creditsTitle.setOutlineThickness(2.f);

    m_creditsBody.setCharacterSize(22u);
    m_creditsBody.setFillColor(sf::Color(220,220,220));
    m_creditsBody.setLineSpacing(1.2f);

    m_creditsBody.setFillColor(sf::Color(220, 220, 220));
    m_creditsBody.setLineSpacing(1.2f);
    
    m_howToTitle.setFillColor(sf::Color(230,230,230));
    m_howToTitle.setOutlineColor(sf::Color(0,0,0,140));
    m_howToTitle.setOutlineThickness(2.f);

    m_howToBody.setFillColor(sf::Color(220,220,220));
    m_howToBody.setLineSpacing(1.2f);
}

void MainMenuScene::onEnter(Game& game)
{
    game.sound().playMusic("res/Assets/Audio/Music/MainMenu.ogg", true);

    if (!m_audioPanel) {
        m_audioPanel = std::make_unique<AudioOptionsPanel>(
            game.sound(),                 
            game.Resources(),             
            "res/Assets/Fonts/PixelifySans-VariableFont_wght.ttf"
        );
        m_audioPanel->set_feedback_sfx("res/Assets/Audio/SFX/SFXFeedback1.ogg");
    }
    
    
    const auto win = game.Window().getSize();
    const float panelW = std::min(560.f, win.x * 0.7f);
    const float panelX = (win.x - panelW) * 0.5f;
    const float panelY = win.y * 0.50f; // mitad de pantalla aprox
    const float cx = static_cast<float>(win.x) * 0.5f;
    const float margin = 192.f;
    m_cullRect = sf::FloatRect({-margin, -margin},
                               {win.x + margin * 2.f, win.y + margin * 2.f});
    
    // --- Fondo ---
    m_bgTex = m_rm.getTexture("res/Assets/Backgrounds/BackGround2.png"); // <-- cambia el path si querés otro
    if (m_bgTex) {
        m_bgTex->setSmooth(false);
        m_bgSprite = std::make_unique<sf::Sprite>(*m_bgTex);
        rebuildBackgroundForWindow(game.Window());
    }
    
    
    m_audioPanel->setOriginAndWidth({ panelX, static_cast<float>(panelY) }, panelW);
    
    const float W = static_cast<float>(win.x);
    const float H = static_cast<float>(win.y);

    // Emisor visual
    const std::string bulletTex = "res/Assets/Sprites/BossBullet1.png";
    const sf::Vector2f bulletScale{0.35f, 0.35f};
    m_emitter = std::make_unique<BulletEmitter>(m_bullets, game.Resources(), bulletTex, bulletScale);

    

    Boss::Params bp;
    bp.patrolBounds = sf::FloatRect(sf::Vector2f(W * 0.1f, 0.f),
                                    sf::Vector2f(W * 0.8f, 1.f));
    bp.startPos = { W * 0.5f, H * 0.4f };
    bp.patrolSpeed = 90.f;
    bp.cdMin = 0.5f; bp.cdMax = 1.5f;
    bp.attackBlockMinP1 = 1.0f;
    bp.attackBlockMaxP1 = 2.0f;
    bp.waveDurationMin = 0.3f;
    bp.waveDurationMax = 0.7f;
    bp.waveGapMin = 0.1f;
    bp.waveGapMax = 0.25f;

    Boss::VisualConfig cfg;
    cfg.sheetPath = "res/Assets/Sprites/Boss/BossSpriteSheet1.png";

    // Fase 1
    cfg.p1.rect  = {{0, 0}, {207, 300}};
    cfg.p1.pivotX = 108.f;
    cfg.p1.baseOffsetY = 200.f;

    // Fase 2
    cfg.p2.rect  = {{207, 0}, {229, 300}};
    cfg.p2.pivotX = 120.f;
    cfg.p2.baseOffsetY = 195.f;

    // Fase 3
    cfg.p3.rect  = {{438, 0}, {220, 300}};
    cfg.p3.pivotX = 115.f;
    cfg.p3.baseOffsetY = 195.f;

    m_boss = Boss::Create(
        game.Resources(),
        bp.startPos,
        1.5f,   // escala
        bp,
        m_emitter.get(),
        nullptr,
        cfg
    );
    
    m_boss->setPlaySfx([sm = &game.sound()](const std::string& path, float pitch, float vol) {
        sm->playSfx(path, pitch, vol);
    });

    bp.patrolBounds = sf::FloatRect(sf::Vector2f(W * 0.1f, 0.f),
                                sf::Vector2f(W * 0.8f, 1.f));
    bp.startPos = { W * 0.5f, H * 0.4f };

    Boss::SfxConfig sfx;
    sfx.straight = "res/Assets/Audio/SFX/AttackNormal.wav";
    sfx.ring = "res/Assets/Audio/SFX/AttackRing.wav";
    sfx.volume = 0.2f;
    m_boss->setSfx(sfx);

    m_bossTimer.restart();
    
    // ---------- BOTON ATRAS  ----------
    m_uiOptions.clear(); // por las dudas
    UIButton& backBtn = m_uiOptions.createButton(
        *m_font, "Atras",
        { static_cast<float>(win.x) * 0.5f, static_cast<float>(panelY) + 120.f },
        { 240.f, 60.f }
    );
    backBtn.setColors(
        sf::Color(88, 76, 140),
        sf::Color(120, 76, 140),
        sf::Color(96, 140, 76)
    );
    backBtn.setTextColor(sf::Color(230, 230, 230));
    backBtn.setOnClick([this]{ m_showOptions = false; });

    m_audioPanel->setCenterX(static_cast<float>(win.x) * 0.60f);
    m_audioPanel->setCenterY(static_cast<float>(win.y) * 0.70f);

    // ---------- HighScore ----------
    std::vector<ScoreEntry> data;
    HighScore::load("res/Saves/HighScore.csv", data);
    
    // Filtra validos y ordenar por tiempo (usa LINQ-templates)
    auto valid  = where(data, [](const ScoreEntry& s){ return s.timeSec > 0.f; });
    auto byTime = order_by(valid, [](const ScoreEntry& s){ return s.timeSec; });
    
    ScoreEntry best = first_or_default(byTime, ScoreEntry{"---", 0.f, 0});

    m_best.setString("Mejor: " + best.initials
                     + "  " + fmtMMSS(best.timeSec)
                     + "  M:" + std::to_string(best.deaths));

    m_best.setPosition({10.f, 10.f});

    std::string topStr;
    const std::size_t TOPN = 5;
    for (std::size_t i = 0; i < std::min(byTime.size(), TOPN); ++i) {
        const auto& e = byTime[i];
        topStr += std::to_string(i+1) + ") " + e.initials + "  "
               + fmtMMSS(e.timeSec) + "  M:" + std::to_string(e.deaths) + "\n";
    }
    if (m_scoresTop) m_scoresTop->setString(topStr);
    
    // Titulo
    m_title.setFont(*m_font); //como uso shared_ptr -> se usa *
    m_title.setString("TwinWire");
    m_title.setCharacterSize(180);
    m_title.setPosition({400.f, 50.f});
    
    const auto winSize = game.Window().getSize();
    const sf::Vector2f center{
        static_cast<float>(winSize.x) * 0.5f,
        static_cast<float>(winSize.y) * 0.5f
    };

    const auto b = m_title.getLocalBounds();
    m_title.setOrigin({b.position.x + b.size.x * 0.5f,
                       b.position.y + b.size.y * 0.5f});
    m_title.setPosition({center.x, static_cast<float>(winSize.y) * 0.15f});

    // ---------- Botones ----------
    // 1) Jugar
    UIButton& playBtn = m_ui.createButton(
        *m_font, "Jugar",
        { center.x, static_cast<float>(winSize.y) * 0.35f },
        { 240.f, 60.f }
    );
    playBtn.setColors(
        sf::Color(88, 76, 140),
        sf::Color(120, 76, 140),
        sf::Color(96, 140, 76)
    );
    playBtn.setTextColor(sf::Color(230, 230, 230));
    playBtn.setOnClick([&game]
    {
        RunStats::reset();
        game.SwitchTo(SceneId::Gameplay);
    });
    s_play.r = makeRectFromCenter(
    { center.x, static_cast<float>(winSize.y) * 0.35f },
    { 240.f, 60.f }
    );
    
    // X) Como jugar
    {
        auto& howToBtn = m_ui.createButton(
        *m_font, "Como jugar",
        { center.x, static_cast<float>(winSize.y) * 0.56f },
        { 240.f, 60.f }
    );
        howToBtn.setColors(
            sf::Color(88, 76, 140),
            sf::Color(120, 76, 140),
            sf::Color(96, 140, 76)
        );
        howToBtn.setTextColor(sf::Color(230, 230, 230));
        howToBtn.setOnClick([this]{ m_showHowTo = true; });
        m_uiHowTo = UIManager{};
        {
            auto& backBtnHowToPlay = m_uiHowTo.createButton(
                *m_font, "Atras",
                { static_cast<float>(winSize.x) * 0.5f, static_cast<float>(winSize.y) * 0.80f },
                { 240.f, 60.f }
            );
            backBtnHowToPlay.setColors(
                sf::Color(88, 76, 140),
                sf::Color(120, 76, 140),
                sf::Color(96, 140, 76)
            );
            backBtnHowToPlay.setTextColor(sf::Color(230, 230, 230));
            backBtnHowToPlay.setOnClick([this]{ m_showHowTo = false; });
        }
        buildHowToTexts();
    }

    // 2) Opciones
    UIButton& optionsBtn = m_ui.createButton(
        *m_font, "Opciones",
        { center.x, static_cast<float>(winSize.y) * 0.45f },
        { 240.f, 70.f }
    );
    optionsBtn.setColors(
        sf::Color(88, 76, 140),
        sf::Color(120, 76, 140),
        sf::Color(96, 140, 76)
    );
    optionsBtn.setTextColor(sf::Color(230, 230, 230));
    optionsBtn.setOnClick([this]{ m_showOptions = true; });
    s_options.r = makeRectFromCenter(
    { center.x, static_cast<float>(winSize.y) * 0.55f },
    { 240.f, 60.f }
    );

    // 3) Créditos
    UIButton& creditsBtn = m_ui.createButton(
        *m_font, "Creditos",
        { center.x, static_cast<float>(winSize.y) * 0.65f },
        { 240.f, 60.f }
    );
    creditsBtn.setColors(
        sf::Color(88, 76, 140),
        sf::Color(120, 76, 140),
        sf::Color(96, 140, 76)
    );
    creditsBtn.setTextColor(sf::Color(230, 230, 230));
    s_credits.r = makeRectFromCenter(
    { center.x, static_cast<float>(winSize.y) * 0.65f },
    { 240.f, 60.f });
    creditsBtn.setOnClick([this]{ m_showCredits = true; });

    m_uiCredits = UIManager{};
    {
        auto& backBtnCredits = m_uiCredits.createButton(
            *m_font, "Atras",
            { static_cast<float>(winSize.x) * 0.5f, static_cast<float>(winSize.y) * 0.80f },
            { 240.f, 60.f }
        );
        backBtnCredits.setColors(
            sf::Color(88, 76, 140),
            sf::Color(120, 76, 140),
            sf::Color(96, 140, 76)
        );
        backBtnCredits.setTextColor(sf::Color(230, 230, 230));
        backBtnCredits.setOnClick([this]{ m_showCredits = false; });

        buildCreditsTexts();
    }
    

    // 4) Salir
    UIButton& exitBtn = m_ui.createButton(
        *m_font, "Salir",
        { center.x, static_cast<float>(winSize.y) * 0.8f },
        { 240.f, 60.f }
    );
    exitBtn.setColors(
        sf::Color(88, 76, 140),
        sf::Color(120, 76, 140),
        sf::Color(96, 140, 76)
    );
    exitBtn.setTextColor(sf::Color(230, 230, 230));
    exitBtn.setOnClick([&game]{ game.RequestQuit(); });
    s_exit.r = makeRectFromCenter(
    { center.x, static_cast<float>(winSize.y) * 0.80f },
    { 240.f, 60.f }
    );
}

void MainMenuScene::onExit(Game& game)
{
    m_boss.reset();
    m_emitter.reset();
    m_bullets.clear();
}

void MainMenuScene::handleEvent(Game& game, const sf::Event& ev)
{
    if (const auto* mb = ev.getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left) {
            const sf::Vector2i mpPix = sf::Mouse::getPosition(game.Window());
            const sf::Vector2f mp    = game.Window().mapPixelToCoords(mpPix);

            const bool pressedOverAny =
                s_play.r.contains(mp)    ||
                s_options.r.contains(mp) ||
                s_credits.r.contains(mp) ||
                s_exit.r.contains(mp);

            if (pressedOverAny) {
                game.sound().playSfx(SFX_PRESS, /*pitch*/1.f, /*vol*/1.f);
            }
        }
    }

    if (m_showHowTo) {
        if (const auto* kp = ev.getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) {
                m_showHowTo = false;
                return;
            }
        }
        m_uiHowTo.handleEvent(ev, game.Window());
        return;
    }

    if (m_showCredits) {
        if (const auto* kp = ev.getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) {
                m_showCredits = false;
                return;
            }
        }
        m_uiCredits.handleEvent(ev, game.Window());
        return;
    }
    
    // ---------- Pantalla de Opciones ----------
    if (m_showOptions) {
        if (const auto* kp = ev.getIf<sf::Event::KeyPressed>()) {
            if (kp->code == sf::Keyboard::Key::Escape) {
                m_showOptions = false;
                return;
            }
        }
        if (m_audioPanel) m_audioPanel->handleEvent(ev, game.Window());
        m_uiOptions.handleEvent(ev, game.Window());
        return;
    }

    // ---------- Menú normal ----------
    if (const auto* key = ev.getIf<sf::Event::KeyPressed>()) {
        if (key->code == sf::Keyboard::Key::Enter) {
            game.SwitchTo(SceneId::Gameplay);
        }
    }

    if (const sf::Event::Resized* r = ev.getIf<sf::Event::Resized>()) {
        const float w = static_cast<float>(r->size.x);
        const float h = static_cast<float>(r->size.y);
        const float margin = 192.f;
        m_cullRect = sf::FloatRect({-margin, -margin}, {w + margin * 2.f, h + margin * 2.f});
        rebuildBackgroundForWindow(game.Window());
    }

    m_ui.handleEvent(ev, game.Window());
}

void MainMenuScene::update(Game& game, float dt)
{
    if (m_showOptions) {
        if (m_audioPanel) m_audioPanel->update(dt);
        m_uiOptions.update(dt);
        return;
    }

    if (m_showHowTo) {
        m_uiHowTo.update(dt);
        return;
    }

    if (m_showCredits) {
        m_uiCredits.update(dt);
        return;
    }

    // --- Boss update ---
    if (m_boss)
    {
        bool dummy = false;
        m_boss->update(dt);
        m_bullets.update(dt, m_cullRect, sf::FloatRect{}, dummy); // sin colisión

       
        static float shootTimer = 0.f;
        shootTimer += dt;

        if (shootTimer >= 0.7f) 
        {
            const sf::Vector2f pos = m_boss->getPosition();
            const float baseAngle = static_cast<float>(std::rand() % 360) * 3.14159f / 180.f;
            const float speed = 300.f + (std::rand() % 200); 

            const int pattern = std::rand() % 3; 

            if (pattern == 0) {
                const int n = 24;
                const float step = 6.2831853f / n;
                for (int i = 0; i < n; ++i)
                    m_emitter->emit(pos, i * step + baseAngle, speed);
            }
            else if (pattern == 1) {
                const int n = 12;
                const float step = 3.14159f / n;
                for (int i = 0; i < n; ++i)
                    m_emitter->emit(pos, i * step + baseAngle, speed);
            }
            else {
                const int n = 10;
                float angle = baseAngle;
                for (int i = 0; i < n; ++i) {
                    m_emitter->emit(pos, angle, speed);
                    angle += 0.35f; // espiral
                }
            }

            shootTimer = 0.f;
        }
    }

    m_ui.update(dt);
}

void MainMenuScene::draw(Game& game, sf::RenderTarget& target)
{
    if (m_bgSprite)
    {
        target.draw(*m_bgSprite);
    }

  
    
    // --- Boss y balas ---
    if (m_boss) target.draw(*m_boss);
    m_bullets.draw(target);
    
    if (m_showOptions) {
        const auto win = game.Window().getSize();
        sf::RectangleShape dim({ float(win.x), float(win.y) });
        dim.setFillColor(sf::Color(0, 0, 0, 140));
        target.draw(dim);

        if (m_audioPanel) m_audioPanel->draw(target);
        m_uiOptions.draw(target);
        return;
    }

    if (m_showHowTo) {
        const auto win = game.Window().getSize();
        sf::RectangleShape dim({ float(win.x), float(win.y) });
        dim.setFillColor(sf::Color(0, 0, 0, 160));
        target.draw(dim);

        // Posicionar textos
        const float cx = win.x * 0.5f;
        m_howToTitle.setPosition({ cx, win.y * 0.1f });
        const auto bt = m_howToTitle.getLocalBounds();
        m_howToTitle.setOrigin({ bt.position.x + bt.size.x * 0.5f, bt.position.y + bt.size.y * 0.5f });

        const float bodyW = std::min(720.f, win.x * 0.78f);
        m_howToBody.setPosition({ cx - bodyW * 0.5f, win.y * 0.2f });

        target.draw(m_howToTitle);
        target.draw(m_howToBody);
        m_uiHowTo.draw(target);
        return;
    }

    if (m_showCredits) {
        const auto win = game.Window().getSize();

        // Fondo oscuro
        sf::RectangleShape dim({ float(win.x), float(win.y) });
        dim.setFillColor(sf::Color(0, 0, 0, 160));
        target.draw(dim);

        // Título centrado
        const float cx = win.x * 0.5f;
        m_creditsTitle.setPosition({ cx, win.y * 0.04f });
        const auto bt = m_creditsTitle.getLocalBounds();
        m_creditsTitle.setOrigin({
            bt.position.x + bt.size.x * 0.5f,
            bt.position.y + bt.size.y * 0.5f
        });
        
        const float bodyW = std::min(720.f, win.x * 0.78f);
        m_creditsBody.setPosition({ cx - bodyW * 0.5f, win.y * 0.06f });

        target.draw(m_creditsTitle);
        target.draw(m_creditsBody);
        m_uiCredits.draw(target);
        return;
    }

    
    target.draw(m_title);
    m_ui.draw(target);
    target.draw(m_best);

    if (m_scoresTop && !m_scoresTop->getString().isEmpty())
        target.draw(*m_scoresTop);
}

void MainMenuScene::rebuildBackgroundForWindow(const sf::RenderWindow& window)
{
    if (!m_bgTex || !m_bgSprite) return;

    const sf::Vector2u tex = m_bgTex->getSize();
    const sf::Vector2u win = window.getSize();

    const float sx = static_cast<float>(win.x) / tex.x;
    const float sy = static_cast<float>(win.y) / tex.y;

    float scale = 1.f;

    if (m_bgPixelPerfect) {
        const bool needUp  = (sx >= 1.f && sy >= 1.f);
        const bool needDown= (sx <  1.f ||  sy <  1.f);

        if (needUp) {
            const float k = std::floor(std::min(sx, sy));
            scale = std::max(1.f, k);
        } else if (needDown) {
            const float kx = std::ceil(static_cast<float>(tex.x) / win.x);
            const float ky = std::ceil(static_cast<float>(tex.y) / win.y);
            const float k  = std::max(kx, ky);
            scale = 1.f / k;
        }
    } else {
        
        scale = std::min(sx, sy);
    }

    m_bgSprite->setScale({ scale, scale });


    const sf::Vector2f bgSize{ tex.x * scale, tex.y * scale };
    const sf::Vector2f pos{ (win.x - bgSize.x) * 0.5f, (win.y - bgSize.y) * 0.5f };
    m_bgSprite->setPosition(pos);
}

void MainMenuScene::buildHowToTexts()
{
    std::ostringstream oss;
    oss
        << "Objetivo\n"
        << " Sobrevive y derrota al boss. Evita proyectiles y usa tus filamentos A/B.\n\n"
        << "Controles\n"
        << " Movimiento: A D\n"
        << " Filamento 1: Click Izquierdo\n"
        << " Filamento 2: Click Derecho\n"
        << " Filamento 1 + 2: Romper pilar\n"
        << " Pausa: ESC\n\n"
        << "Consejos\n"
        << " Algunos patrones cambian de angulo aleatoriamente.\n"
        << "Creditos y Opciones estan disponibles desde este menu.\n";

    m_howToBody.setString(oss.str());
}

void MainMenuScene::buildCreditsTexts()
{
    std::ostringstream oss;
    oss
        << "TwinWire SFML 3.0.0 (C++17)\n\n"
        << "Desarrollo / Programacion:\n"
        << " Alexis Escobar\n\n"
        << "Arte / Ilustracion:\n"
        << " Macarena Vera: Boss, Player\n\n"
        << " Free Sky Backgrounds by Free Game Assets\n"
        << " Free Ancient temple pixel game backgrounds\n"
        << "Musica / SFX:\n"
        << " Touhou Bunkachou (Shoot the Bullet)\n"
        << " Touhou Koumakyou (The Embodiment of Scarlet Devil) \n"
        << " HRtP Shingyokus Theme The Positive and Negative  \n"
        << " Magical Astronomy Track 2 Greenwich in the Sky  \n"
        << " Touhou Phantasmagoria of Dim Dream OST10- Victory Demonstration \n";

    m_creditsBody.setString(oss.str());
}