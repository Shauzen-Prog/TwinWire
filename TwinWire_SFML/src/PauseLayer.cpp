#include "PauseLayer.h"
#include <SFML/Audio.hpp>

#include "RunStats.h"

struct BtnRect { sf::FloatRect r{}; };
static BtnRect s_resumeBtn, s_optionsBtn, s_exitBtn, s_backBtn;

static inline sf::FloatRect makeRectFromCenter(sf::Vector2f c, sf::Vector2f size) {
    return { c - size * 0.5f, size };
}

void PauseLayer::build(const sf::Font& font, const sf::Vector2u& windowSize)
{
    
    // Overlay oscuro
    m_dim.setSize({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    m_dim.setFillColor(sf::Color(0, 0, 0, 180));
    
  
    auto styleButton = [](UIButton& b)
    {
        b.setColors(
            sf::Color(88, 76, 140),   // normal
            sf::Color(120, 76, 140),  // hover
            sf::Color(96, 140, 76)    // pressed
        );
        b.setTextColor(sf::Color(230, 230, 230));
    };

    const float cx = static_cast<float>(windowSize.x) * 0.5f;
    float cy = windowSize.y * 0.5f - 65.f;
    const sf::Vector2f btnSize{ 240.f, 60.f };

    m_ui.clear();

    // Resume
    UIButton& bResume = m_ui.createButton(font, "Reanudar", {cx, cy}, btnSize);
    styleButton(bResume);
    bResume.setOnClick([this]()
    {
        if (m_onResume)  m_onResume();
        hide();
    });
    s_resumeBtn.r = makeRectFromCenter({ cx, cy }, btnSize);
    cy += 120.f;// distancia

    // Options
    UIButton& bOptions = m_ui.createButton(font, "Opciones", {cx, cy}, btnSize);
    styleButton(bOptions);
    bOptions.setOnClick([this]()
    {
        m_showOptions = true;
        if (m_onOptions) m_onOptions();
    });
    s_optionsBtn.r = makeRectFromCenter({ cx, cy }, btnSize);
    cy += 120.f;

    // Exit
    UIButton& bExit = m_ui.createButton(font, "Salir al menu", {cx, cy}, btnSize);
    styleButton(bExit);
    bExit.setOnClick([this]()
    {
        RunStats::reset();
        
        if (m_onExit)
            m_onExit();
    });
    s_exitBtn.r = makeRectFromCenter({ cx, cy }, btnSize);

    m_uiOptions.clear();

    // Panel de audio
    if (!m_audioPanel && m_rm && m_sm) {
        m_audioPanel = std::make_unique<AudioOptionsPanel>(
            *m_sm, *m_rm, m_fontPath
        );
        m_audioPanel->set_feedback_sfx("../Assets/Audio/SFX/SFXFeedback1.ogg");
    }

    if (m_audioPanel) {
        const float panelW = std::min(560.f, windowSize.x * 0.7f);
        const float panelX = (windowSize.x - panelW) * 0.5f;
        const float panelY = windowSize.y * 0.50f;
        m_audioPanel->setOriginAndWidth({ panelX, static_cast<float>(panelY) }, panelW);
    }
    
    {
        const float panelY = windowSize.y * 0.50f;
        UIButton& bBack = m_uiOptions.createButton(
            font, "Atras", { cx, static_cast<float>(panelY) + 120.f }, btnSize
        );
        styleButton(bBack);
        bBack.setOnClick([this]() { m_showOptions = false; });
        s_backBtn.r = makeRectFromCenter({ cx, static_cast<float>(panelY) + 120.f }, btnSize);
    }
}

void PauseLayer::handleEvent(const sf::Event& ev, const sf::RenderWindow& window)
{
    if (!m_visible) return; 
    
    if (const auto* kp = ev.getIf<sf::Event::KeyPressed>()) {
        if (kp->code == sf::Keyboard::Key::Escape) {
            if (m_showOptions) m_showOptions = false;
            else hide();
            return;
        }
    }
    
    if (const auto* mb = ev.getIf<sf::Event::MouseButtonPressed>()) {
        if (mb->button == sf::Mouse::Button::Left && m_sm) {
            const sf::Vector2i mpPix = sf::Mouse::getPosition(window);
            const sf::Vector2f mp = window.mapPixelToCoords(mpPix, window.getDefaultView());

            bool over = false;
            if (!m_showOptions) {
                over = s_resumeBtn.r.contains(mp) || s_optionsBtn.r.contains(mp) || s_exitBtn.r.contains(mp);
            } else {
                over = s_backBtn.r.contains(mp);
            }
            if (over) {
                m_sm->playSfx("../../../../res/Assets/Audio/SFX/ButtonPress.wav", 1.f, 1.f);
            }
        }
    }

    if (m_showOptions) {
        if (m_audioPanel) m_audioPanel->handleEvent(ev, window);
        m_uiOptions.handleEvent(ev, window);
        return;
    }
    
    m_ui.handleEvent(ev, window);
}

void PauseLayer::update(float dt)
{
    if (!m_visible) return;
    
    if (m_showOptions) {
        if (m_audioPanel) m_audioPanel->update(dt);
        m_uiOptions.update(dt);
        return;
    }

    m_ui.update(dt);
}

void PauseLayer::draw(sf::RenderTarget& target) const
{
    if (!m_visible) return;

    const sf::View old = target.getView();
    target.setView(target.getDefaultView());

    target.draw(m_dim);

    if (m_showOptions) {
        if (m_audioPanel) m_audioPanel->draw(target);
        m_uiOptions.draw(target);
    } else {
        m_ui.draw(target);
    }

    target.setView(old);
}
