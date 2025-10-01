#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "ResouceManager.h"   // RM
#include "SpriteAnimator.h"   // usa FrameMeta + pivotX por frame
#include "Filament.h"
#include "IChockeable.h"
#include "IChokeQuery.h"

class Player
{
public:
    explicit Player(ResouceManager& rm, const std::string& sheetPath);
    
    // Setea los frames (rect + pivotX por frame). 
    void setFrames(const std::vector<FrameMeta>& frames, bool loop = true);

    // Loop básico
    void handleInput();                                       // A/D → velocidad X
    void update(float dt, const sf::RenderWindow& window);    // anim + mirar mouse
    void draw(sf::RenderTarget& target) const;

    // Posición
    void setPosition(sf::Vector2f p);
    sf::Vector2f getPosition() const;

    // “Socket” de la mano (local al pivot). 
    void setHandSocketLocal(sf::Vector2f local);
    sf::Vector2f getHandSocketWorld() const;  // origen mundial para el filamento

    // Dirección normalizada hacia el mouse (mundo)
    sf::Vector2f computeShootDirToMouse(const sf::RenderWindow& window) const;

    // Facing
    bool isFacingRight() const { return m_facingRight; }

    void setVisualScale(float s);
    
    // --- Filament API “wrapper” ---
    void startAiming();
    void aimFilamentAt(const sf::Vector2f& targetWorld);
    void confirmAttach(const sf::Vector2f& targetWorld);
    void releaseFilament();

    void setChokeQuery(IChokeQuery* q);
    
private:
    ResouceManager& m_rm;
    ResouceManager::TexturePtr m_sheet;   // spritesheet completo

    bool m_prevRecall = false; 
    
    bool m_prevLeft{false};
    bool m_prevRight{false};
    
    float m_visualScale { 1.f };
    sf::Sprite m_sprite;
    SpriteAnimator m_anim;

    // Transform / movimiento
    sf::Vector2f m_pos { 200.f, 220.f };
    sf::Vector2f m_vel { 0.f, 0.f };
    float m_speed { 220.f };
    bool m_facingRight { true };

    // Socket local (respecto al pivot “talón”: X variable por frame, Y = base del rect)
    sf::Vector2f m_handSocketLocal { 10.f, -22.f }; // ajustar a ojimetro

    // --- Filament ---
    Filament m_filA{3.f};
    Filament m_filB{3.f};
    IChokeQuery* m_chokeQuery{nullptr};

    // Edge detection para mouse (click izq.)
    bool m_prevMouseDownLeft { false };

    // elegir comportamiento: al soltar confirma o suelta
    bool m_confirmOnRelease { true }; // true = attach al soltar; false = release
    
    void updateFacingFromMouse(const sf::RenderWindow& window);
    void applyVisualTransform(); // position + scale (flip)
    
};
