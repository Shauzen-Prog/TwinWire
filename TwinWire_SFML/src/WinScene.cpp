#include "WinScene.h"
#include "RunStats.h"
#include "HighScore.h"
#include "Game.h"
#include <cstdio>

static const char* WIN_BG_PATH   = "../Assets/Backgrounds/BackGround.png";
static const char* WIN_MUSIC_PATH= "../Assets/Audio/Music/Win.ogg";

static std::string fmtMMSS(float t)
{
    int total = static_cast<int>(t + 0.5f);
    int m = total / 60, s = total % 60;
    char buf[16]; std::snprintf(buf, sizeof(buf), "%02d:%02d", m, s);
    return std::string(buf);
}

void WinScene::refreshText()
{
    m_stats.setString("Time " + fmtMMSS(RunStats::elapsed()) + "   Deaths " + std::to_string(RunStats::deaths()));

    std::string shown = m_initials;
    while (shown.size()<3) shown.push_back('_');
    m_initialsText.setString(shown);
}

WinScene::WinScene(ResouceManager& rm)
: m_rm(rm)
, m_font{ m_rm.getFont("../../../../res/Assets/Fonts/Jersey25-Regular.ttf") }
, m_title{ *m_font, "GANASTE!", 80u }
, m_stats{ *m_font, "", 34u }
, m_prompt{ *m_font, "Ingresa tu iniciales (3 letras) Y preciona Enter:", 30u }
, m_initialsText{ *m_font, "___", 34u }
{
    for (sf::Text* t : { &m_title, &m_stats, &m_prompt, &m_initialsText })
    {
        t->setFillColor(sf::Color::White);
        t->setOutlineColor(sf::Color::Black);
        t->setOutlineThickness(2.f);
    }
}

void WinScene::layoutTexts(const sf::Vector2u& winSz)
{
    m_title.setPosition({winSz.x*0.5f - m_title.getLocalBounds().size.x*0.5f, winSz.y*0.2f});
    m_stats.setPosition({winSz.x*0.5f - m_stats.getLocalBounds().size.x*0.5f, winSz.y*0.35f});
    m_prompt.setPosition({winSz.x*0.5f - m_prompt.getLocalBounds().size.x*0.5f, winSz.y*0.5f});
    m_initialsText.setPosition({winSz.x*0.5f - 20.f, winSz.y*0.58f});
}

void WinScene::fitBackgroundToWindow(const sf::Vector2u& winSz)
{
    if (!m_bg || !m_bgTex) return;
    
    const sf::Vector2u texSz = m_bgTex->getSize();
    const float sx = static_cast<float>(winSz.x) / static_cast<float>(texSz.x);
    const float sy = static_cast<float>(winSz.y) / static_cast<float>(texSz.y);
    const float s  = std::max(sx, sy);
    m_bg->setScale({ s, s });
    
    const sf::Vector2f size{ texSz.x * s, texSz.y * s };
    const sf::Vector2f pos{
        (static_cast<float>(winSz.x) - size.x) * 0.5f,
        (static_cast<float>(winSz.y) - size.y) * 0.5f
    };
    m_bg->setPosition(pos);
}


void WinScene::onEnter(Game& game)
{
    RunStats::stop();         // frenar timer del run
    m_initials.clear();
    m_saved = false;
    refreshText();

    if (!m_bgTex) {
        m_bgTex = m_rm.getTexture("../../../../res/Assets/Backgrounds/BackGround.png");
    }
    if (!m_bg && m_bgTex) {
        m_bg.emplace(*m_bgTex);       // <-- construir sprite con la textura
    }

    // --- Fondo (lazy load) ---
    if (!m_bgTex)
    {
        // Carga mediante ResourceManager (cache compartido)
        m_bgTex = m_rm.getTexture(WIN_BG_PATH);
    }
    fitBackgroundToWindow(game.Window().getSize());
    
    game.sound().playMusic(WIN_MUSIC_PATH, /*loop*/ false);

    // --- Layout UI ---
    layoutTexts(game.Window().getSize());
}

void WinScene::onExit(Game& game)
{
    game.sound().stopMusic();
}

void WinScene::handleEvent(Game& game, const sf::Event& ev)
{
    if (const auto* t = ev.getIf<sf::Event::TextEntered>()) {
        if (m_saved) return;
        char32_t ch = t->unicode;
        if (ch >= U'a' && ch <= U'z') ch = ch - (U'a' - U'A');
        if (ch >= U'A' && ch <= U'Z') {
            if (m_initials.size() < 3) {
                m_initials.push_back(static_cast<char>(ch));
                refreshText();
                layoutTexts(game.Window().getSize()); 
            }
        }
        if (ch == 8 /*backspace*/ && !m_initials.empty()){
            m_initials.pop_back();
            refreshText();
            layoutTexts(game.Window().getSize());
        }
    }

    if (const auto* k = ev.getIf<sf::Event::KeyPressed>()) {
        if (k->code == sf::Keyboard::Key::Enter && !m_saved && m_initials.size()==3) {
            std::vector<ScoreEntry> data;
            const std::string hsPath = "../Saves/HighScore.csv";
            HighScore::load(hsPath, data);
            HighScore::insertAndSort(data, ScoreEntry{m_initials, RunStats::elapsed(), RunStats::deaths()});
            HighScore::save(hsPath, data);
            m_saved = true;

            // Volver a Main Menu 
            game.SwitchTo(SceneId::MainMenu);
        }
    }
    
    if (const sf::Event::Resized* r = ev.getIf<sf::Event::Resized>()) {
        const sf::Vector2u winSz{ r->size.x, r->size.y };
        fitBackgroundToWindow(winSz);
        layoutTexts(winSz);
    }
}

void WinScene::handleInput(Game& game){}

void WinScene::update(Game& game, float dt){}

void WinScene::draw(Game& game, sf::RenderTarget& rt)
{
    
    if (m_bg) rt.draw(*m_bg);
    
    rt.draw(m_title);
    rt.draw(m_stats);
    rt.draw(m_prompt);
    rt.draw(m_initialsText);
}


