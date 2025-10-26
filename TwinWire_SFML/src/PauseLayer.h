#pragma once
#include <SFML/Graphics.hpp>
#include <functional>

#include "SoundManager.h"
#include "UIManager.h"
#include "AudioOptionsPanel.h"

class PauseLayer
{
public:
    PauseLayer() = default;

    void build(const sf::Font& font, const sf::Vector2u& windowSize);
    void handleEvent(const sf::Event& ev, const sf::RenderWindow& window);
    void update(float dt);
    void draw(sf::RenderTarget& target) const;

    // Visibilidad
    bool isVisible() const {return m_visible;}
    void show() {m_visible = true;}
    void hide() {m_visible = false;}
    void toggle() {m_visible = !m_visible;}

    // Callbacks
    void setOnResume(std::function<void()> cb)  { m_onResume  = std::move(cb); }
    void setOnOptions(std::function<void()> cb) { m_onOptions = std::move(cb); }
    void setOnExit(std::function<void()> cb)    { m_onExit    = std::move(cb); }

    void setSoundManager(SoundManager* sm) { m_sm = sm; }
    void setResourceManager(ResouceManager* r) { m_rm = r; }             
    void setFontPath(std::string path)         { m_fontPath = std::move(path); } 

    // Control de subvista Opciones
    void showOptions(bool v = true) { m_showOptions = v; }
    bool optionsVisible() const { return m_showOptions; }

    void setPanelCenterX(float cx) { if (m_audioPanel) m_audioPanel->setCenterX(cx); }
    void setPanelTopY(float y)     { if (m_audioPanel) m_audioPanel->setTopY(y); }
    
private:
    SoundManager* m_sm{ nullptr };
    ResouceManager* m_rm{ nullptr };
    
    bool m_visible{false};
    bool m_showOptions{false};  
  
    UIManager m_ui;
    UIManager m_uiOptions;  
    sf::RectangleShape m_dim;

    std::unique_ptr<AudioOptionsPanel> m_audioPanel;
    std::string m_fontPath = "res/Assets/Fonts/PixelifySans-VariableFont_wght.ttf";

    // CallBacks de los botones
    std::function<void()> m_onResume;
    std::function<void()> m_onOptions;
    std::function<void()> m_onExit;
    
};
