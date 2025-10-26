#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "IWidget.h"
#include "VolumeSlider.h"
#include "SoundManager.h"

class AudioOptionsPanel : public IWidget
{
public:
    explicit AudioOptionsPanel(SoundManager& sm, ResouceManager& rm, const std::string& fontPath);
    
    void setOriginAndWidth(sf::Vector2f topLeft, float width);
    void set_feedback_sfx(const std::string& path);

    void setCenterX(float cx, bool snap = true);
    void nudgeX(int dx);
    
    void setCenterY(float cy, bool snap = true);
    void setTopY(float y, bool snap = true);
    void nudgeY(int dy);

    // IWidget
    void handleEvent(const sf::Event& ev, const sf::RenderWindow& window) override;
    void update(float dt) override;
    void draw(sf::RenderTarget& target) const override;

private:
    SoundManager& m_sm;
    VolumeSlider m_master;
    VolumeSlider m_sfx;
    VolumeSlider m_music;

    ResouceManager::FontPtr m_font;
    bool m_hasFont{false};
    sf::Text m_lblMaster;
    sf::Text m_lblSfx;
    sf::Text m_lblMusic;
    sf::Text m_title;

    sf::FloatRect m_rMaster{}, m_rSfx{}, m_rMusic{};
    std::string m_feedbackSfx;
    bool m_draggingSfx{false};
    float m_lastSfxValue{-1.f};

    sf::Vector2f m_pos{};
    float m_width{360.f};
    float m_rowH{28.f};

    void rebuildLayout();
    void syncFromMixer();
    void syncToMixer();
};
