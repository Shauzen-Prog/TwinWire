#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include <optional>

#include "IScene.h"

enum class SceneId { MainMenu, Gameplay };

class Game
{
public:
    Game();
    ~Game();

    // Main loop
    void Play();

    // Facade: request to quit from any scene
    void RequestQuit() { m_wantsQuit = true; }

    // Facade: scene switching
    void SwitchTo(SceneId id);

    // Minimal access for scenes
    sf::RenderWindow& Window()             { return *m_window; }
    const sf::RenderWindow& Window() const { return *m_window; }
    
private:
    // ---- subsystems hidden from scenes
    std::unique_ptr<sf::RenderWindow> m_window;
    sf::Clock  m_clock;
    bool       m_wantsQuit { false };

    // ---- scene management
    std::unordered_map<SceneId, std::unique_ptr<IScene>> m_scenes;
    std::optional<SceneId> m_current;

    // lifecycle
    void initialize();
    void deinitialize();
    void createWindow();
    void destroyWindow();
    void registerScenes();

    // game loop steps
    void handleEvents();
    void handleInput();
    void update(float dt);
    void draw();
};
