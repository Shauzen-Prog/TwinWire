#pragma once
#include <SFML/Graphics.hpp>
class Game;

class IScene
{
public:
    virtual ~IScene() = default;

    virtual void onEnter(Game& game) {}
    virtual void onExit(Game& game) {}

    //Eventos
    virtual void handleEvent(Game& game, const sf::Event& ev) {}

    //Input en tiempo real
    virtual void handleInput(Game& game) {}

    //Logica frame a frame
    virtual void update(Game& game, float dt) = 0;

    //Draw
    virtual void draw(Game& game, sf::RenderTarget& rt) = 0;
};
