#include "AudioOptionsPanel.h"

#include <iostream>

AudioOptionsPanel::AudioOptionsPanel(SoundManager& sm, ResouceManager& rm, const std::string& fontPath)
: m_sm(sm)
, m_font(rm.getFont(fontPath))                                            
, m_lblMaster(*m_font, "Master", 18u)                                     
, m_lblSfx   (*m_font, "SFX",    18u)
, m_lblMusic (*m_font, "Music",  18u)
, m_title    (*m_font, "Audio Options", 20u)
{
    m_hasFont = (m_font != nullptr);
    
    syncFromMixer();
    rebuildLayout();
}

void AudioOptionsPanel::setOriginAndWidth(sf::Vector2f topLeft, float width)
{
    m_pos = topLeft;
    m_width = width;
    rebuildLayout();
}

void AudioOptionsPanel::set_feedback_sfx(const std::string& path)
{ m_feedbackSfx = path; }

void AudioOptionsPanel::setCenterX(float cx, bool snap)
{
    float x = cx - (m_width * 0.5f);
    if (snap) x = std::round(x);
    m_pos.x = x;
    rebuildLayout();
}

void AudioOptionsPanel::nudgeX(int dx)
{
    m_pos.x = std::round(m_pos.x + static_cast<float>(dx));
    rebuildLayout();
}

void AudioOptionsPanel::setCenterY(float cy, bool snap)
{
    float y = cy - /* altura virtual del bloque */ 0.f; 
    if (snap) y = std::round(y);
    m_pos.y = y;
    rebuildLayout();
}

void AudioOptionsPanel::setTopY(float y, bool snap)
{
    if (snap) y = std::round(y);
    m_pos.y = y;
    rebuildLayout();
}

void AudioOptionsPanel::nudgeY(int dy)
{
    m_pos.y = std::round(m_pos.y + static_cast<float>(dy));
    rebuildLayout();
}

void AudioOptionsPanel::rebuildLayout()
{
    auto snap = [](float v) { return std::round(v); };
    
    const float barH = 14.f;  
    const float gapY = 70.f;  
    const float padX = 250.f;
    const float above = 18.f;

    const float xSlider = m_pos.x;           
    const float y0 = m_pos.y;        
    const float sliderW = m_width - padX;    
    
    m_rMaster ={ { xSlider, y0 + -4.f * gapY }, { sliderW, barH } };
    m_rSfx    ={ { xSlider, y0 + -3.f * gapY }, { sliderW, barH } };
    m_rMusic  ={ { xSlider, y0 + -2.f * gapY }, { sliderW, barH } };

    m_master.setRect(m_rMaster);
    m_sfx.setRect(m_rSfx);
    m_music.setRect(m_rMusic);
    
    if (!m_hasFont) return;
    
    m_lblMaster.setString("Master");
    m_lblSfx.setString   ("SFX");
    m_lblMusic.setString ("Musica");

    for (sf::Text* t : { &m_lblMaster, &m_lblSfx, &m_lblMusic }) {
        const auto b = t->getLocalBounds();
        t->setOrigin({ b.position.x + b.size.x * 0.5f, b.position.y + b.size.y });
    }
    
    const float cx = xSlider + sliderW * 0.5f;
    m_lblMaster.setPosition({ cx, m_rMaster.position.y - above });
    m_lblSfx.setPosition   ({ cx, m_rSfx.position.y    - above });
    m_lblMusic.setPosition ({ cx, m_rMusic.position.y  - above });
    
}

void AudioOptionsPanel::syncFromMixer()
{
    m_master.setValue(m_sm.masterVolume());
    m_sfx.setValue(m_sm.sfxVolume());
    m_music.setValue(m_sm.musicVolume());
}

void AudioOptionsPanel::syncToMixer()
{
    m_sm.setMasterVolume(m_master.value());
    m_sm.setSfxVolume(m_sfx.value());
    m_sm.setMusicVolume(m_music.value());
}

void AudioOptionsPanel::handleEvent(const sf::Event& ev, const sf::RenderWindow& window)
{
    m_master.handleEvent(ev, window);
    m_sfx.handleEvent(ev, window);
    m_music.handleEvent(ev, window);
    
    if (const auto* mp = ev.getIf<sf::Event::MouseButtonPressed>()) {
        if (mp->button == sf::Mouse::Button::Left) {
            const sf::Vector2i pix = sf::Mouse::getPosition(window);
            const sf::Vector2f p   = window.mapPixelToCoords(pix);
            m_draggingSfx  = m_rSfx.contains(p);
            m_lastSfxValue = m_sfx.value(); // valor al empezar el drag
        }
    }
    
    if (const auto* mr = ev.getIf<sf::Event::MouseButtonReleased>()) {
        if (mr->button == sf::Mouse::Button::Left) {
            syncToMixer();
            m_sm.saveVolumes();

            if (m_draggingSfx && !m_feedbackSfx.empty()) {
                if (std::abs(m_sfx.value() - m_lastSfxValue) > 0.01f) {
                    m_sm.playSfx(m_feedbackSfx, 1.f, 0.85f);
                }
            }
            m_draggingSfx = false;
        }
    }
}
void AudioOptionsPanel::update(float /*dt*/)
{
    syncToMixer();
}

void AudioOptionsPanel::draw(sf::RenderTarget& target) const
{
    m_master.draw(target);
    m_sfx.draw(target);
    m_music.draw(target);
    
    if (m_hasFont) {
        target.draw(m_lblMaster);
        target.draw(m_lblSfx);
        target.draw(m_lblMusic);
    }
}
