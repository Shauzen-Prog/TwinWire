#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "ResouceManager.h"   // RM
#include "SpriteAnimator.h"   // usa FrameMeta + pivotX por frame
#include "Filament.h"
#include "IChockeable.h"
#include "IChokeQuery.h"

enum class AnimId { Idle, Walk, LHSustain, LHRelease, RHSustain, RHRelease, Die };

class Player
{
public:
    enum Hand { Left, Right };
    
    struct Tuning {
        sf::Vector2f pivotOffset   { 8.f, 0.f };   // “pies” global (X,Y)
        float visualScale   = 2.2f;         // escala del sprite
        sf::Vector2f hbSize        {10.f, 22.f};   // AABB size
        sf::Vector2f hbOffset      { 1.f, -9.f };   // AABB offset (desde pies)

        sf::Vector2f socketRightLocal { 31.f, 19.f };
        sf::Vector2f socketLeftLocal  { 26.f,20.f };
    };

    
    explicit Player(ResouceManager& rm, const std::string& sheetPath);
    
    // Setea los frames (rect + pivotX por frame). 
    void setFrames(const std::vector<FrameMeta>& frames, bool loop = true);

    // Loop básico
    void handleEvent(const sf::Event& ev, const sf::RenderWindow& window);
    void handleInput();                                       // A/D → velocidad X
    void update(float dt, const sf::RenderWindow& window);    // anim + mirar mouse
    void draw(sf::RenderTarget& target) const;

    // Lecturas
    sf::Vector2f getFeetWorld() const;
    sf::FloatRect aabb() const;
    bool isFacingRight() const { return m_facingRight; }

    // Animator
    const SpriteAnimator& animator() const { return m_anim; }
    SpriteAnimator& animator() { return m_anim; }

    // Posición
    
    sf::Vector2f getPosition() const;

    // Sockets
    sf::Vector2f handSocketLocal(Hand h) const;
    void setHandSocketLocal(Hand h, sf::Vector2f p);
    void adjustHandSocket(Hand h, sf::Vector2f d);
    sf::Vector2f handSocketWorld(Hand h) const;

    // Tuning Fijo
    const Tuning& tuning() const { return m_tuning; }

    // API visual
    void setPosition(sf::Vector2f p);
    void setVisualScale(float s);

    // Dirección normalizada hacia el mouse (mundo)
    sf::Vector2f computeShootDirToMouse(const sf::RenderWindow& window, Hand h) const;
    
    inline void Player_playReleaseForHand(Player& self, Hand h);

    // Hitbox getters/setters (runtime tuning)
    sf::Vector2f hitboxSize()   const { return m_tuning.hbSize; }
    sf::Vector2f hitboxOffset() const { return m_tuning.hbOffset; }
    void setHitboxSize(sf::Vector2f s);
    void setHitboxOffset(sf::Vector2f o);
    void adjustHitboxSize(sf::Vector2f delta);
    void adjustHitboxOffset(sf::Vector2f delta);
    
    // --- Filament API “wrapper” ---
    void startAiming();
    void aimFilamentAt(const sf::Vector2f& targetWorld);
    void confirmAttach(const sf::Vector2f& targetWorld);
    void releaseFilament();

    void setChokeQuery(IChokeQuery* q);

    
    void setTuning(const Tuning& t);
    void setPivotOffset(sf::Vector2f p) { m_tuning.pivotOffset = p; m_anim.setPivotOffset(p); }


   

    // Animacion Getter
    
    
    void play(AnimId id, bool loop = true, bool holdLast = false);
    void playLoopLastFrame(AnimId id);

    // Velocidad get y setter
    void setSpeed(float s) { m_speed = s; }
    float speed() const { return m_speed; }

    std::unordered_map<AnimId, std::vector<FrameMeta>> m_frames; // cache
    void lockMovement(float seconds, float slowFactor = 0.f);
   

private:
    ResouceManager& m_rm;
    ResouceManager::TexturePtr m_sheet;   // spritesheet completo
    Tuning m_tuning;

    bool m_prevRecall = false; 
    
    bool m_prevLeft{false};
    bool m_prevRight{false};

    sf::Vector2f m_spawnPos { 160.f, 160.f};
    
    float m_visualScale { 1.f };
    sf::Sprite m_sprite;
    SpriteAnimator m_anim;

    // Transform / movimiento
    
    float m_speed { 200.f }; // px/s
    float m_velX { 0.f }; 
    sf::Vector2f m_pos { 200.f, 220.f };
    sf::Vector2f m_vel { 0.f, 0.f };
    bool m_facingRight { true };

    // Movement gate
    float m_moveLockTimer { 0.f}; // >0 significa bloqueado/relentizado
    float m_moveSlowFactor {1.f}; // 0 = bloqueo total, 0.2 = 20% de velocidad, etc
    
    bool isMovementLocked() const { return m_moveLockTimer > 0.f; }

    // Anim cache para no re-setear lo mismo
    AnimId m_currentAnim { AnimId::Idle };

    void playIfChanged(AnimId id, bool loop, bool holdLast = false);
    void updateFacingFromVelocity();
    
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
