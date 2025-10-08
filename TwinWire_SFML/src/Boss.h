#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <random>

#include "Helpers.h"
#include "IOrb.h"
#include "StateMachine.h"

class IBulletEmitter; // forward

class Boss final : public sf::Drawable
{
public:
    enum Phase { P1, P2}; // fases
    enum State { Patrol, Pause, BulletHell, CheckOrb, SeekOrb, Absorb, Hurt, Dead };

    struct Params
    {
        sf::FloatRect patrolBounds; // Limites en X
        float patrolSpeed = 120.f;
        float seekSpeed = 200.f;
        float pauseTime = 1.f;
        float absorbTime = 8.f;
        float alignEpsilon = 4.f;
        sf::Vector2f startPos{};
        sf::Vector2f size {48.f,32};
        float cdMin = 2.5f;
        float cdMax = 4.5f;

        float attackBlockMinP1 = 2.5f;   // presupuesto total de EMISIÓN en P1
        float attackBlockMaxP1 = 4.0f;
        float attackBlockMinP2 = 4.0f;   // en P2 ataca mas
        float attackBlockMaxP2 = 6.0f;

        float waveDurationMin = 0.6f;    // cada oleada dura entre X e Y
        float waveDurationMax = 1.2f;
        float waveGapMin = 0.25f;   // pausa corta entre oleadas
        float waveGapMax = 0.6f;

        float gapWidthMinDeg  = 30.f;
        float gapWidthMaxDeg  = 65.f;
        float gapStepMinDeg   = 4.f;   // cuanto se mueve el pasillo por ring (en grados)
        float gapStepMaxDeg   = 9.f;
        float spinPerRingMinDeg = 0.f; // 0 = spokes perfectamente alineados
        float spinPerRingMaxDeg = 3.f; 

        // --- Pattern Tuning ---
        // Ring (P1/P2)
        int ringMin = 16;
        int ringMax = 24;
        float ringSpeedP1 = 500.f;
        float ringSpeedP2 = 800.f;

        // ToPlayer (snapshot)
        float toPlayerSpeedP1 = 260.f;
        float toPlayerSpeedP2 = 320.f;
        float toPlayerPulseMin = 0.18f;
        float toPlayerPulseMax = 0.28f;

        // RingLoop con gap (P2)
        float ringIntervalMin = 0.10f;
        float ringIntervalMax = 0.18f;

        // Beam tuning
        float beamBaseThickness = 3.f; // grosor base
        float beamPulseAmp = 2.f; // cuánto late
        float beamPulseHz = 6.f; // frecuencia del pulso (veces por segundo)
        sf::Color beamColorP1 { 100, 27,  245, 255 }; // violeta en P1
        sf::Color beamColorP2 { 255,  80,  90, 255 }; // rojizo en P2
    };

    Boss(const Params& p,
        IBulletEmitter* emitter,
        const std::vector<IOrb*>* orbs); // Necesito saber si es nulleable por eso hago el <IOrb*>*

    void update(float dt);
    Phase phase() const { return m_phase; }
    sf::Vector2f getPosition() const { return m_position; }
    // Para ToPlayer(snapshot): le doy al boss una forma de obtener la pos del player sin acoplar tanto
    void setPlayerPosProvider(std::function<sf::Vector2f()> provider) { m_getPlayerPos = std::move(provider); }

    // Helper publico
    bool isDead() const { return m_isDead; } 
    void setOnDeath(std::function<void()> cb) {m_onDeath = std::move(cb); }
    
private:

    using FSM = StateMachine<State, Boss>; // Le doy el onwner al boss
    
    // FSM callbacks
    void onEnterPatrol();               void onUpdatePatrol(float dt);
    void onEnterPause();                void onUpdatePause(float dt);
    void onEnterBulletHell();           void onUpdateBulletHell(float dt); void onExitBulletHell();
    void onEnterCheckOrb();
    void onUpdateSeekOrb(float dt);
    void onEnterAbsorb();               void onUpdateAbsorb(float dt);
    void onEnterHurt();                 void onUpdateHurt(float dt);
    void onEnterDead();
    void onUpdateDead(float dt);

    // ---- Waves & Patterns ----
    enum class WavePattern { RingOnce, ToPlayerPulse, RingLoopGap }; // expandible por si quiero meter mas en un futuro

    struct WaveCtx {
        WavePattern pattern { WavePattern::RingOnce };
        // comunes
        float timeToNextPulse { 0.f }; // para ToPlayerPulse o ringInterval
        // ToPlayer
        sf::Vector2f playerSnapshot{};
        // RingLoopGap
        float gapCenter { 0.f };   // en rad
        float gapHalf   { 0.f };   // en rad
        int   ringsFired{ 0 };
        float ringInterval { 0.f };  // intervalo entre anillos de esta wave
        float ringStartAngle { 0.f };// angulo base de los “spokes”
        float gapStepPerRing { 0.f };// cuanto se desplaza el gap por anillo (rad)
        float spinPerRing { 0.f };   // cuanto gira el anillo por ring (rad). 0 = spokes alineados
    };

    void beginWave();          // elige patron segun fase y configura WaveCtx
    void updateWave(float dt); // ejecuta el patron actual durante la wave
    void choosePatternForPhase();  // decide WaveCtx.pattern

    // Helpers de emision
    void emitRing(int count, float startAngleRad, float speed, bool useGap,
                  float gapCenterRad, float gapHalfRad);
    void emitToPlayerPulse(float speed); // usa WaveCtx.playerSnapshot y timeToNextPulse
    
    // Helpers
    void selectClosestActiveOrb();
    bool hasActiveOrbs() const;
    void phaseChange();
    void resetAttackCooldown();
    void updateBodyVisual();
    void updateBeamVisual();

    // Beam variables
    float m_beamPulseT { 0.f }; // tiempo acumulado para el pulso
    sf::RectangleShape m_beamGlow; // "halo" detras del beam
    
    void clearBeam();
    void updateBeamStyle(float dt); // (solo estilos: grosor/glow)

    // Draw
    void draw(sf::RenderTarget& target, sf::RenderStates states) const override; // se que no hace falta el override pero me gusta y me queda mas claro
    
    // ------------- Variables privadas ------------
   
    FSM m_fsm;

    Params m_cfg;
    IBulletEmitter* m_emitter { nullptr };
    const std::vector<IOrb*>* m_orbs { nullptr };
    std::function<sf::Vector2f()> m_getPlayerPos;
    
    Phase m_phase { Phase::P1 };

    // Helper si muere
    bool m_isDead{false};
    std::function<void()> m_onDeath;

    //Movement / pos
    sf::Vector2f m_position{};
    float m_dirX { +1.f };

    // Timers
    float m_pauseTimer {0.f};
    float m_absorbTimer {0.f};
    float m_attackCooldown {0.f};

    // Attack Block
    float m_attackBudget { 0.f };   // tiempo total de EMISION restante
    float m_waveTimer { 0.f };      // tiempo restante de la oleada actual
    float m_waveGapTimer { 0.f };   // tiempo de pausa entre oleadas
    bool  m_emittingWave { false }; // true = disparando, false = en pausa entre oleadas

    // Vida del boss
    int m_hitsTaken{0};
    int m_hitsToKill{2}; // 2 golpes: 1 pasa a P2, 2 = muerte

    WaveCtx m_wave{};         // estado por wave
    std::mt19937 m_rng{ std::random_device{}() }; // una manera de hacer random
    
    // Target orb (referencia debil / Notas en docs)
    IOrb* m_currentOrb { nullptr };

    // Visuales
    sf::RectangleShape m_body;
    sf::RectangleShape m_beam;
};
