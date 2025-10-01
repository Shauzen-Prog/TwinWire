#include "Filament.h"
#include "IChockeable.h"
#include <cmath>

Filament::Filament(float thickness)
: m_state(*this)
{
    m_beam.setSize({0.f, thickness});
    m_beam.setOrigin({0.f, thickness * 0.5f});
    m_beam.setFillColor(sf::Color(200,220,255));

    using S = FilamentState;

    // --- IDLE ---
    m_state.registerState(S::Idle, {
        /*enter*/  [=](Filament& f){
            f.m_beam.setSize({0.f, f.m_beam.getSize().y});
            f.m_attached      = false;
            f.m_attachedObj   = nullptr;
        },
        /*update*/ [](Filament&, float){},
        /*exit*/   [](Filament&){}
    });

    // --- EXTENDING: m_lockedOrigin -> m_lockedOrigin + dir*maxLen ---
    m_state.registerState(S::Extending, {
    /*enter*/  [=](Filament& /*f*/){ /* m_extendT=0; set por fireStraight */ },
    /*update*/ [](Filament& f, float dt)
    {
        // mantener la base donde se disparó
        if (f.m_lockOriginOnExtend) f.m_origin = f.m_lockedOrigin;

        // longitud propuesta para este frame
        const float newLen = std::min(f.m_lenPx + f.m_extendSpeed * dt, f.m_maxLength);

        // segmento del frame: de la punta anterior a la nueva punta
        const sf::Vector2f prevTip = f.m_lockedOrigin + f.m_dir * f.m_lenPx;
        const sf::Vector2f nextTip = f.m_lockedOrigin + f.m_dir * newLen;

        // ---- RAYCAST EN ESTE TRAMO ----
        if (f.m_raycast) {
            sf::Vector2f hit{};
            if (IChockeable* c = f.m_raycast(prevTip, nextTip, hit)) {
                // adjuntar en el primer contacto
                f.m_attached    = true;
                f.m_attachedObj = c;
                f.m_attachPoint = hit;
                f.m_target      = hit;
                f.refreshBeam();
                f.m_state.change(S::Attached);
                return;
            }
        }

        // Sin impacto: avanzar
        f.m_lenPx  = newLen;
        f.m_target = nextTip;
        f.refreshBeam();

        // Alcanzo el máximo sin pegar → retraer
        if (f.m_lenPx >= f.m_maxLength) {
            f.m_state.change(S::Retracting);
        }
    }
    });

    // --- ATTACHED: punta fija en attachPoint; origen sigue la mano (no lock) ---
    m_state.registerState(S::Attached, {
    /*enter*/  [](Filament& /*f*/){},
    /*update*/ [](Filament& f, float){
        f.m_target = f.m_attachPoint;
        f.m_beam.setFillColor(sf::Color::Green);
        f.refreshBeam();
    },
    /*exit*/   [](Filament& f){ f.m_attached=false; f.m_attachedObj=nullptr; }
    });

    // --- RETRACTING: desde la punta actual hacia la mano bloqueada ---
    using S = FilamentState;
    m_state.registerState(S::Retracting, {
        /*enter*/  [](Filament& /*f*/){},
        /*update*/ [](Filament& f, float dt){
            // base fija mientras retrae
            if (f.m_lockOriginOnExtend) f.m_origin = f.m_lockedOrigin;

            // avanzar la punta hacia la mano a px/s
            const sf::Vector2f back = f.m_lockedOrigin - f.m_target;
            const float dist  = std::sqrt(back.x*back.x + back.y*back.y);
            const float step  = f.m_retractSpeed * dt;

            if (dist <= 1e-5f || step >= dist) {
                f.m_target = f.m_lockedOrigin;
                f.refreshBeam();
                f.m_state.change(S::Cooldown); // arranca el cooldown
                return;
            }

            f.m_target += (back / dist) * step;
            f.refreshBeam();
        },
        /*exit*/   [](Filament&){}
    });

    // --- COOLDOWN → IDLE ---
    m_state.registerState(S::Cooldown, {
     /*enter*/  [](Filament& f){
         f.m_beam.setSize({0.f, f.m_beam.getSize().y});
         f.m_cooldown = f.m_cooldownTime; // arranca aca el cdr
     },
     /*update*/ [](Filament& f, float dt){
         f.m_cooldown -= dt;
         if (f.m_cooldown <= 0.f) f.m_state.change(S::Idle);
     },
     /*exit*/   [](Filament&){}
 });
    
    m_state.change(S::Idle);
}

Filament::Filament() : Filament(3.f) {}

void Filament::fireStraight(const sf::Vector2f& origin, const sf::Vector2f& mouseWorld, bool canAttach)
{
    if (!canFire()) return;
    
    m_canAttach    = canAttach;
    m_attached     = false;
    m_attachedObj  = nullptr;

    // 1) Origen congelado del disparo
    m_startOrigin = origin;
    m_origin      = origin; // por compatibilidad

    // 2) DIRECCION HORIZONTAL SOLAMENTE
    //    Tomo el mouse solo para elegir el signo: derecha o izquierda.
    const float sx = (mouseWorld.x >= origin.x) ? 1.f : -1.f;
    m_dir = { sx, 0.f };               

    // 3) Preparo el lerp horizontal a maxLength
    m_startTarget = origin;
    m_finalTarget = origin + m_dir * m_maxLength;

    // 4) Kick visual y flags
    m_extendT   = 0.f;
    m_retractT  = 0.f;
    m_extending = true;
    m_retracting= false;

    m_target = origin + m_dir * 2.f;       // 2 px para que se vea al instante
    refreshBeam();

    m_state.change(FilamentState::Extending);
}

void Filament::updateOrigin(const sf::Vector2f& world)
{
    if (m_extending) return;   // ignorar mientras se estira
    m_origin = world;
}

void Filament::update(float dt) {

    
    // cooldown 
    if (m_cooldown > 0.f) {
        m_cooldown -= dt;
        if (m_cooldown < 0.f) m_cooldown = 0.f;
    }

    // ---------- EXTENDING ----------
    if (m_extending) {
        // fija la base durante la extensión
        if (m_lockOriginOnExtend) m_origin = m_startOrigin;

        const sf::Vector2f diff = m_finalTarget - m_startTarget;
        const float totalLen = std::max(vlen(diff), 1e-5f);

        const float tPrev = m_extendT;
        const float tNext = std::min(1.f, tPrev + (m_extendSpeed * dt) / totalLen);

        const sf::Vector2f prevTip = m_startTarget + diff * tPrev;
        const sf::Vector2f nextTip = m_startTarget + diff * tNext;

        // ---- RAYCAST SOLO EN ESTE TRAMO ----
        if (m_raycast && m_canAttach) {
            sf::Vector2f hit{};
            if (IChockeable* c = m_raycast(prevTip, nextTip, hit)) {
                // adjuntar en el primer impacto
                m_attached    = true;
                m_attachedObj = c;
                m_attachPoint = hit;
                m_target      = hit;

                m_extending   = false;
                m_state.change(FilamentState::Attached); // solo informativo
                refreshBeam();
                return;
            }
        }

        // avanzar si no hubo impacto
        m_extendT = tNext;
        m_target  = nextTip;
        refreshBeam();

        // llego al maximo sin pegar -> retraer
        if (tNext >= 1.f) {
            m_extending   = false;
            m_retractT    = 0.f;
            m_retracting  = true;
            m_state.change(FilamentState::Retracting); // informativo
        }
    }

    // ---------- RETRACTING ----------
    if (m_retracting) {
        // volver hacia la mano del disparo
        const sf::Vector2f back = m_startOrigin - m_target;
        const float dist  = std::max(vlen(back), 1e-5f);
        const float step  = m_retractSpeed * dt;

        if (step >= dist) {
            m_retracting = false;
            m_target     = m_startOrigin;
            refreshBeam();
            m_cooldown   = m_cooldownTime;      // <- arranca CD aca
        } else {
            m_target += (back / dist) * step;
            refreshBeam();
        }
    }
}

void Filament::draw(sf::RenderTarget& target) const {

    if (m_beam.getSize().x > 0.5f) target.draw(m_beam);
}

void Filament::forceRetract()
{
    // Si no hay nada visible, sale
    if (m_beam.getSize().x <= 0.5f && !m_attached && !m_extending && !m_retracting)
        return;

    // Soltar cualquier attach y pasar a retraer
    m_attached    = false;
    m_attachedObj = nullptr;

    m_extending   = false;
    m_retractT    = 0.f;
    m_retracting  = true;

    // Base fija durante la retracción
    if (m_lockOriginOnExtend) m_origin = m_startOrigin;

    m_state.change(FilamentState::Retracting);
}


void Filament::refreshBeam()
{
    // usar el origen del disparo mientras extiende/retrae
    const sf::Vector2f base = (m_extending || m_retracting) ? m_startOrigin : m_origin;
    const sf::Vector2f d = m_target - base;
    const float L = vlen(d);

    m_beam.setPosition(base);
    if (L <= 1e-4f) { m_beam.setSize({0.f, m_beam.getSize().y}); return; }

    const float ang = std::atan2(d.y, d.x);
    m_beam.setRotation(sf::radians(ang)); // SFML 3: se puede usar radianes :)
    m_beam.setSize({ L, m_beam.getSize().y });
}








