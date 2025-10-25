#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include <optional>
#include "SoundManager.h"
#include "IScene.h"
#include "ResouceManager.h" 

enum class SceneId { MainMenu, Gameplay, WinScene, PlayerTuner };

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

    void reloadCurrentScene();

    // Minimal access for scenes
    sf::RenderWindow& Window()             { return *m_window; }
    const sf::RenderWindow& Window() const { return *m_window; }

    SoundManager& sound()             { return m_sound; }
    const SoundManager& sound() const { return m_sound; }
    
    ResouceManager& Resources()       { return m_res; }
    const ResouceManager& Resources() const { return m_res; }
    
private:
    // ---- subsystems hidden from scenes
    std::unique_ptr<sf::RenderWindow> m_window;
    sf::Clock  m_clock;
    bool       m_wantsQuit { false };

    // ---- scene management
    std::unordered_map<SceneId, std::unique_ptr<IScene>> m_scenes;
    std::optional<SceneId> m_current;

    ResouceManager m_res;
    SoundManager   m_sound{ m_res, 48 };

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
