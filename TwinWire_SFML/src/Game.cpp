#include "Game.h"

#include <fstream>
#include <iostream>

#include "MainMenuScene.h"
#include "GameplayScene.h"
#include "PlayerTunerScene.h"
#include "WinScene.h"

Game::Game()  { initialize(); }
Game::~Game() { deinitialize(); }

void Game::initialize()
{
    createWindow();
    
    // --- Persistencia simple a archivo local ---
    m_sound.setSaveVolumesCallback([](float M, float Mu, float S){
        std::ofstream f("audio.cfg");
        if (f) f << M << " " << Mu << " " << S;
    });
    m_sound.setLoadVolumesCallback([](float& M, float& Mu, float& S)->bool{
        std::ifstream f("audio.cfg");
        if (!f) return false;
        return (f >> M >> Mu >> S) ? true : false;
    });
    
    m_sound.loadVolumes();
    registerScenes();
    //arranca en el menu
    SwitchTo(SceneId::MainMenu);
}

void Game::deinitialize()
{
    // sale de la escena actual para clerear
    if (m_current)
    {
        if (auto it = m_scenes.find(*m_current); it != m_scenes.end() && it->second)
            it->second->onExit(*this);
        m_current.reset();
    }

    m_scenes.clear();
    destroyWindow();
}

void Game::createWindow()
{
    unsigned int windowWidth = 1280;
    unsigned int windowHeight = 720;
    
    m_window = std::make_unique<sf::RenderWindow>(sf::VideoMode({windowWidth,windowHeight}), "TwinWire");
    m_window->setFramerateLimit(60);
}

void Game::destroyWindow()
{
    if (m_window)
    {
        if (m_window->isOpen()) m_window->close();
        m_window.reset();
    }
}

void Game::registerScenes()
{
    m_scenes.emplace(SceneId::MainMenu, std::make_unique<MainMenuScene>());
    
    m_scenes.emplace(SceneId::Gameplay, std::make_unique<GameplayScene>("../Assets/Sprites/Player/PlayerSpriteSheet.png"));

    m_scenes.emplace(SceneId::WinScene, std::make_unique<WinScene>(m_res));

    m_scenes.emplace(SceneId::PlayerTuner, std::make_unique<PlayerTunerScene>("../Assets/Sprites/Player/PlayerSpriteSheet.png"));
    
}

void Game::reloadCurrentScene()
{
    if (!m_current) return;
    auto it = m_scenes.find(*m_current);
    if (it == m_scenes.end() || !it->second) return;
    it->second->onExit(*this);
    it->second->onEnter(*this);
}

void Game::Play()
{
    while (m_window->isOpen() && !m_wantsQuit)
    {
        const float dt = m_clock.restart().asSeconds();

        handleEvents(); 
        handleInput();  
        update(dt);     
        draw();          
    }
}

void Game::handleEvents()
{
    while (const std::optional<sf::Event> event = m_window->pollEvent())
    {
        const sf::Event e = *event;  // ← COPIA LOCAL, segura el scope

        if (e.is<sf::Event::Closed>()) {
            m_window->close();
            continue;
        }

        if (m_current) {
            if (auto it = m_scenes.find(*m_current); it != m_scenes.end() && it->second)
                it->second->handleEvent(*this, e); // ← paso la COPIA, no *event
        }
    }
}

void Game::handleInput()
{
    if (!m_current) return;
    if (auto it = m_scenes.find(*m_current); it != m_scenes.end() && it->second)
        it->second->handleInput(*this);
}

void Game::update(float dt)
{
    m_sound.update();
    if (!m_current) return;
    if (auto it = m_scenes.find(*m_current); it != m_scenes.end() && it->second)
        it->second->update(*this, dt);
}

void Game::draw()
{
    m_window->clear(sf::Color(20, 20, 28));

    if (m_current)
    {
        if (auto it = m_scenes.find(*m_current); it != m_scenes.end() && it->second)
            it->second->draw(*this, *m_window);
    }

    m_window->display();
}

void Game::SwitchTo(SceneId id)
{
    if (m_current && *m_current == id) return;

    auto itNew = m_scenes.find(id);
    if (itNew == m_scenes.end() || !itNew->second)
    {
        std::cerr << "[Game] SwitchTo(): scene not registered\n";
        return; 
    }
    
    if (m_current)
    {
        if (auto it = m_scenes.find(*m_current); it != m_scenes.end() && it->second)
            it->second->onExit(*this);
    }
    
    m_current = id;
    itNew->second->onEnter(*this);
}