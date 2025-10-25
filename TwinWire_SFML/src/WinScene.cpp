#include "WinScene.h"
#include "RunStats.h"
#include "HighScore.h"
#include "Game.h"
#include <cstdio>

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
, m_title{ *m_font, "GANASTE!", 28u }
, m_stats{ *m_font, "", 18u }
, m_prompt{ *m_font, "Ingresa tu iniciales (3 letras) Y preciona Enter:", 16u }
, m_initialsText{ *m_font, "___", 24u }
{
    for (sf::Text* t : { &m_title, &m_stats, &m_prompt, &m_initialsText })
    {
        t->setFillColor(sf::Color::White);
        t->setOutlineColor(sf::Color::Black);
        t->setOutlineThickness(2.f);
    }
}

void WinScene::onEnter(Game& game)
{

    RunStats::stop();         // frenar timer del run
    m_initials.clear();
    m_saved = false;
    refreshText();

    const auto winSz = game.Window().getSize();
    m_title.setPosition({winSz.x*0.5f - m_title.getLocalBounds().size.x*0.5f, winSz.y*0.2f});
    m_stats.setPosition({winSz.x*0.5f - m_stats.getLocalBounds().size.x*0.5f, winSz.y*0.35f});
    m_prompt.setPosition({winSz.x*0.5f - m_prompt.getLocalBounds().size.x*0.5f, winSz.y*0.5f});
    m_initialsText.setPosition({winSz.x*0.5f - 60.f, winSz.y*0.58f});
}

void WinScene::onExit(Game& game){}

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
            }
        }
        if (ch == 8 /*backspace*/ && !m_initials.empty()){
            m_initials.pop_back();
            refreshText();
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
}

void WinScene::handleInput(Game& game){}

void WinScene::update(Game& game, float dt){}

void WinScene::draw(Game& game, sf::RenderTarget& rt)
{
    rt.draw(m_title);
    rt.draw(m_stats);
    rt.draw(m_prompt);
    rt.draw(m_initialsText);
}


