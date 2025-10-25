#include "MainMenuScene.h"
#include <iostream>
#include "Game.h"
#include <cstdio>

#include "HighScore.h"
#include "linq.h"

static std::string fmtMMSS(float t)
{
    int total = static_cast<int>(t + 0.5f);
    int m = total / 60, s = total % 60;
    char buf[16]; std::snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
    return std::string(buf);
}

MainMenuScene::MainMenuScene()
: m_font(m_rm.getFont("../Assets/Fonts/PixelifySans-VariableFont_wght.ttf")),
m_title(*m_font, "", 30),
m_best{ *m_font, "", 18u }
{
    // Creo la semilla del random
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    m_best.setFillColor(sf::Color::White);
    m_best.setOutlineColor(sf::Color::Black);
    m_best.setOutlineThickness(2.f);
    m_best.setPosition({10.f, 10.f});
}

void MainMenuScene::onEnter(Game& game)
{
    game.sound().playMusic("../Assets/Audio/Music/MainMenu.ogg", true);
    
    if (!m_audioPanel) {
        m_audioPanel = std::make_unique<AudioOptionsPanel>(
            game.sound(),                 
            game.Resources(),             
            "../Assets/Fonts/PixelifySans-VariableFont_wght.ttf"
        );
        m_audioPanel->setFeedbackSfx("../Assets/Audio/SFX/SFXFeedback.wav");
    }

    
    const auto win = game.Window().getSize();
    const float panelW = std::min(560.f, win.x * 0.7f);
    const float panelX = (win.x - panelW) * 0.5f;
    const float panelY = win.y * 0.50f; // mitad de pantalla aprox
    const float cx = static_cast<float>(win.x) * 0.5f;
    m_audioPanel->setOriginAndWidth({ panelX, static_cast<float>(panelY) }, panelW);
    
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
    HighScore::load("../Saves/HighScore.csv", data);
    // Filtra validos y ordenar por tiempo (usa LINQ-templates)
    auto valid  = where(data, [](const ScoreEntry& s){ return s.timeSec > 0.f; });
    auto byTime = order_by(valid, [](const ScoreEntry& s){ return s.timeSec; });
    ScoreEntry best = first_or_default(byTime, ScoreEntry{"---", 0.f, 0});

    m_best.setString("Mejor: " + best.initials
                     + "  " + fmtMMSS(best.timeSec)
                     + "  M:" + std::to_string(best.deaths));

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
    playBtn.setOnClick([&game]{ game.SwitchTo(SceneId::Gameplay); });

    // 2) Opciones
    UIButton& optionsBtn = m_ui.createButton(
        *m_font, "Opciones",
        { center.x, static_cast<float>(winSize.y) * 0.5f },
        { 240.f, 60.f }
    );
    optionsBtn.setColors(
        sf::Color(88, 76, 140),
        sf::Color(120, 76, 140),
        sf::Color(96, 140, 76)
    );
    optionsBtn.setTextColor(sf::Color(230, 230, 230));
    optionsBtn.setOnClick([this]{ m_showOptions = true; });

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
}

void MainMenuScene::onExit(Game& game)
{
    
}

void MainMenuScene::handleEvent(Game& game, const sf::Event& ev)
{
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

    m_ui.update(dt);
}

void MainMenuScene::draw(Game& game, sf::RenderTarget& target)
{
    if (m_showOptions) {
        const auto win = game.Window().getSize();
        sf::RectangleShape dim({ float(win.x), float(win.y) });
        dim.setFillColor(sf::Color(0, 0, 0, 140));
        target.draw(dim);

        if (m_audioPanel) m_audioPanel->draw(target);
        m_uiOptions.draw(target);
        return;
    }
    
    target.draw(m_title);
    m_ui.draw(target);
    target.draw(m_best);
}