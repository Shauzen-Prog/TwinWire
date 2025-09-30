#include "MainMenuScene.h"
#include <iostream>
#include "Game.h"

MainMenuScene::MainMenuScene()
    : m_font(res.getFont("../Assets/Fonts/PixelifySans-VariableFont_wght.ttf")),
    m_title(*m_font, "", 30)
{
    
}

void MainMenuScene::onEnter(Game& game)
{
    
    // Titulo
    m_title.setFont(*m_font); //como uso shared_ptr -> se usa *
    m_title.setString("TwinWire");
    m_title.setCharacterSize(180);
    m_title.setPosition({400.f, 50.f});

    //lo centro
    const auto winSize = game.Window().getSize();
    const sf::Vector2f center{ static_cast<float>(winSize.x) * 0.5f,
                                static_cast<float>(winSize.y) * 0.5f };
    {
        const auto b = m_title.getLocalBounds();
        m_title.setOrigin({b.position.x + b.size.x * 0.5f,
        b.position.y + b.size.y * 0.5f});
        m_title.setPosition({center.x, static_cast<float>(winSize.y) * 0.15f});
    }

    // ---- Botones ----
    //1) Jugar
    UIButton& playBtn = m_ui.createButton(
        *m_font,
        "Jugar",
        { center.x, static_cast<float>(winSize.y) * 0.35f },
        { 240.f, 64.f}
        );
    playBtn.setOnClick([&game]{
    
        game.SwitchTo(SceneId::Gameplay);
    });

    //2) Opciones
    auto& optionsBtn = m_ui.createButton(
        *m_font, "Opciones",
        { center.x, static_cast<float>(winSize.y) * 0.5f },
        { 240.f, 64.f}
        );
    optionsBtn.setOnClick([&game]{
        //game.SwitchTo(SceneId::Options);
    });

    //3) Creditos
    auto& creditsBtn = m_ui.createButton(
        *m_font, "Creditos",
        { center.x, static_cast<float>(winSize.y) * 0.65f },
        { 240.f, 64.f}
        );
    creditsBtn.setOnClick([&game]{
        //game.SwitchTo(SceneId::Credits);
    });

    //4) Salir
    auto& exitBtn = m_ui.createButton(
        *m_font, "Salir",
        { center.x, static_cast<float>(winSize.y) * 0.8f },
        { 240.f, 64.f}
        );
    exitBtn.setOnClick([&game]{
        game.RequestQuit();
    });
}

void MainMenuScene::onExit(Game& game)
{
    std::cout << "Saliendo de MainMenu\n";
}

void MainMenuScene::handleEvent(Game& game, const sf::Event& ev)
{
    if (const auto* key = ev.getIf<sf::Event::KeyPressed>())
    {
        if (key->code == sf::Keyboard::Key::Enter)
        {
            game.SwitchTo(SceneId::Gameplay);
        }
    }

    m_ui.handleEvent(ev, game.Window());
}

void MainMenuScene::update(Game& game, float dt)
{
    // Nada por ahora
    m_ui.update(dt);
}

void MainMenuScene::draw(Game& game, sf::RenderTarget& target)
{
    target.draw(m_title);
    m_ui.draw(target);
}