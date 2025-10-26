#include "Boss.h"
#include "GeometryUtils.h"
#include "Helpers.h"
#include <algorithm>
#include <random>
#include <cmath>
#include "IBulletEmitter.h"
#include "SoundManager.h"

static float deg2rad(float d) { return d * 3.14159265358979323846f / 180.f; }

static inline sf::Vector2f rectSize(const sf::IntRect& r)
{
    return static_cast<sf::Vector2f>(r.size);
}

sf::Color Boss::lerp(const sf::Color& a, const sf::Color& b, float t) const
{
    t = std::clamp(t, 0.f, 1.f);
    auto L = [&](uint8_t x, uint8_t y) -> uint8_t
    {
        return static_cast<uint8_t>(x + (y - x) * t);
    };

    return {L(a.r, b.r), L(a.g, b.g), L(a.b, b.b), L(a.a, b.a)};
}

// los constructores son los mismos que en c# solo que se declaran de esta manera y te obliga
Boss::Boss( ResouceManager& rm,
            const std::string& texturePath,
            std::optional<sf::IntRect> rect,
            sf::Vector2f worldPos,
            float scale,
            const Params& p,
            IBulletEmitter* emitter,
            const std::vector<IOrb*>* orbs)
:m_rm(rm),
m_tex(m_rm.getTexture(texturePath)),
m_sprite(*m_tex),
m_fsm(*this), m_cfg(p), m_emitter(emitter), m_orbs(orbs)
{
    m_tex->setSmooth(true);

    if (rect) m_sprite.setTextureRect(*rect);

    const sf::Vector2f sz = rect ? rectSize(*rect)
                                : static_cast<sf::Vector2f>(m_sprite.getTextureRect().size);
    m_sprite.setOrigin({ sz.x * 0.5f, sz.y });
    m_sprite.setPosition(worldPos);
    m_sprite.setRotation(sf::Angle::Zero);
    m_sprite.setScale({ scale, scale });

    m_sprite.setColor(sf::Color::White);
    
    m_position = p.startPos;

    m_body.setSize(p.size);
    m_body.setOrigin(p.size * 0.5f);
    m_body.setFillColor(sf::Color(180, 60, 60));

    // Beam principal
    m_beam.setOrigin({0.f, m_cfg.beamBaseThickness * 0.5f});
    m_beam.setSize({0.f, m_cfg.beamBaseThickness});
    m_beam.setFillColor(m_cfg.beamColorP1);

    // Beam Glow
    m_beamGlow.setOrigin({0.f, (m_cfg.beamBaseThickness * 1.8f) * 0.5f});
    m_beamGlow.setSize({0.f, (m_cfg.beamBaseThickness * 1.8f)});
    sf::Color glowColor = m_cfg.beamColorP1;
    glowColor.a = 70; // alpha suave
    m_beamGlow.setFillColor(glowColor);

    // Regitro los callbacks del FSM
    m_fsm.addState(State::Patrol, {
                       /*enter*/ [](Boss& b) { b.onEnterPatrol(); },
                       /*update*/[](Boss& b, float dt) { b.onUpdatePatrol(dt); },
                       /*exit*/ nullptr,
                       /*event*/ nullptr
                   });
    m_fsm.addState(State::Pause, {
                       [](Boss& b) { b.onEnterPause(); },
                       [](Boss& b, float dt) { b.onUpdatePause(dt); },
                       nullptr,
                       nullptr
                   });
    m_fsm.addState(State::BulletHell, {
                       [](Boss& b) { b.onEnterBulletHell(); },
                       [](Boss& b, float dt) { b.onUpdateBulletHell(dt); },
                       [](Boss& b) { b.onExitBulletHell(); },
                       nullptr
                   });
    m_fsm.addState(State::CheckOrb, {
                       [](Boss& b) { b.onEnterCheckOrb(); },
                       nullptr,
                       nullptr,
                       nullptr
                   });
    m_fsm.addState(State::SeekOrb, {
                       nullptr,
                       [](Boss& b, float dt) { b.onUpdateSeekOrb(dt); },
                       nullptr,
                       nullptr
                   });
    m_fsm.addState(State::Absorb, {
                       [](Boss& b) { b.onEnterAbsorb(); },
                       [](Boss& b, float dt) { b.onUpdateAbsorb(dt); },
                       nullptr,
                       nullptr
                   });
    m_fsm.addState(State::Hurt, {
                       [](Boss& b) { b.onEnterHurt(); },
                       [](Boss& b, float dt) { b.onUpdateHurt(dt); },
                       nullptr,
                       nullptr
                   });
    m_fsm.addState(State::Dead, {
                       [](Boss& b) { b.onEnterDead(); },
                       [](Boss& b, float dt) { b.onUpdateDead(dt); },
                       nullptr,
                       nullptr
                   });

    m_fsm.setInitial(State::Patrol);
    resetAttackCooldown();
}


void Boss::startReload(float seconds)
{
    m_flash = FlashMode::ReloadViolet;
    m_fxTime = 0.f;
    m_fxDur = std::max(0.f, seconds);
}

void Boss::update(float dt)
{
    if (m_isDead) { updateBodyVisual(); return;}
    
    m_fsm.update(dt); 
    updateBodyVisual();
    updateColorFX(dt);
}

void Boss::setPhase(int p)
{
    const int k = std::clamp(p,1,3);
    m_phase = static_cast<Phase>(k);
    applyRect();
}

void Boss::setPhase(Phase ph)
{
    m_phase = ph;
    
    applyRect();
}

void Boss::onDamage(int amount)
{
    m_flash = FlashMode::DamageRed;
    m_fxTime = 0.f;
    m_fxDur = 0.15f;
}

void Boss::applyRect()
{
    if (!m_hasPhaseRects) return;

    sf::IntRect r{};
    const PhaseVisual* pv = nullptr;
    switch (m_phase) {
    case Phase::P1: pv = &m_vcfg.p1; break;
    case Phase::P2: pv = &m_vcfg.p2; break;
    case Phase::P3: pv = &m_vcfg.p3; break;
    default:        pv = &m_vcfg.p1; break;
    }

    m_sprite.setTextureRect(pv->rect);
    
    const sf::Vector2f sz = static_cast<sf::Vector2f>(pv->rect.size);
    const sf::Vector2f oldPos = m_sprite.getPosition();
    
    m_sprite.setOrigin({ pv->pivotX, sz.y - pv->baseOffsetY });

    m_sprite.setPosition(oldPos);
}


void Boss::updateColorFX(float dt)
{
    if (m_flash == FlashMode::None) return;

    m_fxTime += dt;
    const float t = std::clamp(m_fxTime / std::max(0.0001f, m_fxDur), 0.f, 1.f);

    if (m_flash == FlashMode::DamageRed) {
        // Rojo -> Blanco
        float s = 0.5f + 0.5f * std::sin(t * 6.28318f * 3.f);
        const sf::Color c = lerp(sf::Color::White, sf::Color(255,64,64), s);
        m_sprite.setColor(c);
        if (t >= 1.f) m_flash = FlashMode::None;
    } else if (m_flash == FlashMode::ReloadViolet) {
        // Blanco -> Violeta durante la recarga al final vuelve a blanco
        const sf::Color c = lerp(sf::Color::White, sf::Color(180, 90, 255), t);
        m_sprite.setColor(c);
        if (t >= 1.f) {
            m_sprite.setColor(sf::Color::White);
            m_flash = FlashMode::None;
        }
    }
}


void Boss::playSfx_(const std::string& p, float pitch, float vol)
{
    if (m_playSfx && !p.empty())
        m_playSfx(p, pitch, std::clamp(m_sfx.volume * vol, 0.f, 1.f));
}

void Boss::setPhaseRects(const PhaseRects& pr)
{
    m_rects = pr;
    m_hasPhaseRects = true;
    applyRect();
}


void Boss::onEnterPatrol()
{
    // nada de momento
}
void Boss::onUpdatePatrol(float dt)
{
    m_position.x += m_dirX * m_cfg.patrolSpeed * dt;

    const float left  = RECT_LEFT(m_cfg.patrolBounds);
    const float right = RECT_RIGHT(m_cfg.patrolBounds);
    if (m_position.x < left)  { m_position.x = left;  m_dirX = +1.f; }
    if (m_position.x > right) { m_position.x = right; m_dirX = -1.f; }

    m_attackCooldown -= dt;
    if (m_attackCooldown <= 0.f) m_fsm.change(State::Pause);
}

void Boss::onEnterPause()
{
    m_pauseTimer = m_cfg.pauseTime;   // 1.0f por default
    m_beam.setSize({0.f, 4.f});
    m_body.setFillColor(sf::Color(200, 120, 120));
}

void Boss::onUpdatePause(float dt)
{
    m_pauseTimer -= dt;
    if (m_pauseTimer <= 0.f) {
        m_fsm.change(State::BulletHell);
    }
}

// ———————————————————————————————
// onEnterBulletHell: budget y 1ª wave
// ———————————————————————————————
void Boss::onEnterBulletHell() {
    // presupuesto total de EMISIÓN (solo cuenta cuando m_emittingWave = true)
    std::uniform_real_distribution<float> budgetDist(
        (m_phase == Phase::P1) ? m_cfg.attackBlockMinP1 : m_cfg.attackBlockMinP2,
        (m_phase == Phase::P1) ? m_cfg.attackBlockMaxP1 : m_cfg.attackBlockMaxP2
    );
    m_attackBudget = budgetDist(m_rng);

    m_emittingWave = true;
    beginWave(); // elige patrón y configura timers/ctx
}


// ———————————————————————————————
// onUpdateBulletHell: ejecuta wave / pausa / siguiente wave o salir
// ———————————————————————————————
void Boss::onUpdateBulletHell(float dt) {
    if (!m_emitter) {
        // sin emisor: consumimos presupuesto y salimos igual
        m_attackBudget -= dt;
        if (m_attackBudget <= 0.f) m_fsm.change(State::CheckOrb);
        return;
    }

    if (m_emittingWave) {
        // Estamos "emitiendo" (puede haber pulsos internos)
        m_attackBudget -= dt;
        m_waveTimer    -= dt;

        updateWave(dt); // ejecuta el patrón actual

        if (m_attackBudget <= 0.f || m_waveTimer <= 0.f) {
            // termina esta wave
            m_emittingWave = false;

            if (m_attackBudget <= 0.f) {
                m_fsm.change(State::CheckOrb);
                return;
            }

            // pausa corta entre waves
            std::uniform_real_distribution<float> gapDist(m_cfg.waveGapMin, m_cfg.waveGapMax);
            m_waveGapTimer = gapDist(m_rng);
        }
    } else {
        // Pausa entre waves
        m_waveGapTimer -= dt;
        if (m_waveGapTimer <= 0.f) {
            m_emittingWave = true;
            beginWave(); // nueva wave (elige patrón en función de la fase)
        }
    }
}

void Boss::onExitBulletHell()
{
    // if (m_emitter) m_emitter->stop(); hay que probar si no se rompe igual con esto
}

void Boss::onEnterCheckOrb()
{
    if (hasActiveOrbs()) {
        selectClosestActiveOrb();
        if (m_currentOrb) { m_fsm.change(State::SeekOrb); return; }
    }
    
    resetAttackCooldown();
    m_fsm.change(State::Patrol);
}

void Boss::onUpdateSeekOrb(float dt)
{
    // Checks para que no se rompa, si se rompe el orbe
    if (!m_currentOrb || !m_currentOrb->isActive())
    {
        resetAttackCooldown();
        m_fsm.change(State::Patrol);
        return;
    }

    const float targetX = m_currentOrb->getPosition().x;
    const float dx = targetX - m_position.x;

    if (std::abs(dx) > m_cfg.alignEpsilon)
    {
        const float s = (dx > 0.f) ? 1.f : -1.f;
        m_position.x += s * m_cfg.seekSpeed * dt;

        const float left = RECT_LEFT(m_cfg.patrolBounds);
        const float right = RECT_RIGHT(m_cfg.patrolBounds);
        m_position.x = std::max(left, std::min(right, m_position.x));
    }
    else
    {
        m_fsm.change(State::Absorb);
    }
}

void Boss::onEnterAbsorb()
{
    m_absorbTimer = m_cfg.absorbTime;
    m_beamPulseT = 0.f;

    playSfx_(m_sfx.absorbStart, 1.f, 1.f);

    //color por fase
    const sf::Color col = (m_phase == Phase::P1) ? m_cfg.beamColorP1 : m_cfg.beamColorP2;
    m_beam.setFillColor(col);
    sf::Color glow = col; glow.a = 70;
    m_beam.setOutlineColor(glow);
    
    updateBeamVisual(); // calcula longitud y rotacion
    updateBeamStyle(0.f); // aplica grosor/glow inicial 
    
}

void Boss::onUpdateAbsorb(float dt)
{
    if (!m_currentOrb || !m_currentOrb->isActive())
    {
        clearBeam();
        // Si el orbe es destruido mientras "absorbe" -> aplica daño y cambia de fase
        m_currentOrb = nullptr;
        ++m_hitsTaken;
        if (m_hitsTaken >= m_hitsToKill)
        {
            // Si se acercan mas de 3 veces al orbe, se considera muerto
            m_fsm.change(State::Dead);
            return;
        }
        // primer golpe pasa a P2 y hace Hurt
        phaseChange();
        m_fsm.change(State::Hurt);
        return;
    }
    
    m_absorbTimer -= dt;
    if (m_absorbTimer <= 0.f)
    {
        clearBeam();
        // Sin daño (el orbe sobrevivio al tiempo)
        m_currentOrb = nullptr;
        resetAttackCooldown();
        m_fsm.change(State::Patrol);
        return;
    }

    updateBeamVisual();
    updateBeamStyle(dt);
}

void Boss::onEnterHurt()
{
    playSfx_(m_sfx.hurt, 1.f, 1.f);
    m_currentOrb = nullptr;
}

void Boss::onUpdateHurt(float dt)
{
    resetAttackCooldown();
    m_fsm.change(State::Patrol);
}

void Boss::onEnterDead()
{
    m_isDead = true;
    m_beam.setSize({0.f, 4.f});
    m_body.setFillColor(sf::Color(60, 60, 60));
    if (m_onDeath) m_onDeath();
}

void Boss::onUpdateDead(float dt)
{
    // nada de momento
    // aca va animacion de muerte seguramente
}

void Boss::beginWave() {

    switch (m_wave.pattern)
    {
    case WavePattern::ToPlayerPulse:
        playSfx_(m_sfx.straight, 1.f, 1.f);
        break;
    case WavePattern::RingLoopGap:
    case WavePattern::RingOnce:
        playSfx_(m_sfx.ring, 1.f, 1.f);
        break;
    }
    
    // Duracion de esta wave
    std::uniform_real_distribution<float> durDist(m_cfg.waveDurationMin, m_cfg.waveDurationMax);
    m_waveTimer = durDist(m_rng);

    choosePatternForPhase();

    // Reset estado interno por patron
    m_wave.timeToNextPulse = 0.f;
    m_wave.ringsFired = 0;

    if (m_wave.pattern == WavePattern::ToPlayerPulse) {
        // Foto de la posicion del player al inicio de la wave
        m_wave.playerSnapshot = m_getPlayerPos ? m_getPlayerPos() : m_position;
       
        // delay inicial para el primer disparo
        std::uniform_real_distribution<float> rate(m_cfg.toPlayerPulseMin, m_cfg.toPlayerPulseMax);
        m_wave.timeToNextPulse = rate(m_rng);
    }
    else if (m_wave.pattern == WavePattern::RingLoopGap) {
        // Gap ancho:
        std::uniform_real_distribution<float> gapW(deg2rad(m_cfg.gapWidthMinDeg),
                                                   deg2rad(m_cfg.gapWidthMaxDeg));
        // Gap centro inicial (cualquier dirección):
        std::uniform_real_distribution<float> gapC(-3.14159265f, +3.14159265f);
        // Intervalo entre anillos:
        std::uniform_real_distribution<float> ringI(m_cfg.ringIntervalMin, m_cfg.ringIntervalMax);
        // Cuanto se corre el gap por anillo:
        std::uniform_real_distribution<float> gapStep(deg2rad(m_cfg.gapStepMinDeg),
                                                      deg2rad(m_cfg.gapStepMaxDeg));
        // Cuanto gira el ring por anillo (0 => spokes fijos):
        std::uniform_real_distribution<float> spinStep(deg2rad(m_cfg.spinPerRingMinDeg),
                                                       deg2rad(m_cfg.spinPerRingMaxDeg));
        // Angulo base de los spokes 
        std::uniform_real_distribution<float> base(-3.14159265f, +3.14159265f);

        m_wave.gapHalf      = 0.5f * gapW(m_rng);
        m_wave.gapCenter    = gapC(m_rng);
        m_wave.ringInterval = ringI(m_rng);
        m_wave.gapStepPerRing = gapStep(m_rng);
        m_wave.spinPerRing  = spinStep(m_rng);
        m_wave.ringStartAngle = base(m_rng);

        m_wave.timeToNextPulse = m_wave.ringInterval; // primer ring
        m_wave.ringsFired = 0;
    }
    else {
        // RingOnce: dispara un anillo apenas comienza la wave
        int n = std::uniform_int_distribution<int>(m_cfg.ringMin, m_cfg.ringMax)(m_rng);
        float start = std::uniform_real_distribution<float>(-3.14159265f, +3.14159265f)(m_rng);
        float speed = (m_phase == Phase::P1) ? m_cfg.ringSpeedP1 : m_cfg.ringSpeedP2;
        emitRing(n, start, speed,  false, 0.f, 0.f);

        // un cooldown chico para no spamear
        m_wave.timeToNextPulse = 9999.f; // no mas pulsos en esta wave (solo 1 ring)
    }
}

void Boss::updateWave(float dt) {
    switch (m_wave.pattern) {
    case WavePattern::ToPlayerPulse: {
        m_wave.timeToNextPulse -= dt;
        if (m_wave.timeToNextPulse <= 0.f) {
            float speed = (m_phase == Phase::P1) ? m_cfg.toPlayerSpeedP1 : m_cfg.toPlayerSpeedP2;
            emitToPlayerPulse(speed);
            // siguiente pulso
            std::uniform_real_distribution<float> rate(m_cfg.toPlayerPulseMin, m_cfg.toPlayerPulseMax);
            m_wave.timeToNextPulse = rate(m_rng);
        }
    } break;

    case WavePattern::RingLoopGap: {
        m_wave.timeToNextPulse -= dt;
        if (m_wave.timeToNextPulse <= 0.f) {
            const int   n      = std::uniform_int_distribution<int>(m_cfg.ringMin, m_cfg.ringMax)(m_rng);
            const float speed  = m_cfg.ringSpeedP2; // P2 mas rapido

            // Emite el ring respetando el pasillo:
            emitRing(n, m_wave.ringStartAngle, speed,
                     true, m_wave.gapCenter, m_wave.gapHalf);

            // Avances para el proximo ring:
            m_wave.ringsFired++;
            m_wave.gapCenter += m_wave.gapStepPerRing;          // el pasillo se corre un poquito
            if (m_wave.gapCenter >  3.14159265f) m_wave.gapCenter -= 6.28318531f;
            if (m_wave.gapCenter < -3.14159265f) m_wave.gapCenter += 6.28318531f;

            m_wave.ringStartAngle += m_wave.spinPerRing;        // spokes fijos si 0
            if (m_wave.ringStartAngle >  3.14159265f) m_wave.ringStartAngle -= 6.28318531f;
            if (m_wave.ringStartAngle < -3.14159265f) m_wave.ringStartAngle += 6.28318531f;

            // Proximo ring:
            m_wave.timeToNextPulse = m_wave.ringInterval;
        }
    } break;

    case WavePattern::RingOnce:
    default:
        // Nada que hacer el ring ya se emitio en beginWave
        break;
    }
}

void Boss::choosePatternForPhase() {
    if (m_phase == Phase::P1) {
        // P1: RingOnce o ToPlayerPulse 50/50
        int r = std::uniform_int_distribution<int>(0, 1)(m_rng);
        m_wave.pattern = (r == 0) ? WavePattern::RingOnce : WavePattern::ToPlayerPulse;
    } else {
        // P2: RingLoopGap (acorralar). Se puede elegir pesos
        m_wave.pattern = WavePattern::RingLoopGap;
    }
}

// Helpers
void Boss::emitRing(int count, float startAngleRad, float speed, bool useGap,
                    float gapCenterRad, float gapHalfRad) {
    if (!m_emitter || count <= 0) return;

    const float step = 6.28318531f / static_cast<float>(count);
    for (int i = 0; i < count; ++i) {
        const float a = startAngleRad + step * static_cast<float>(i);

        if (useGap) {
           
            float d = a - gapCenterRad;
            // normalizo
            while (d >  3.14159265f) d -= 6.28318531f;
            while (d < -3.14159265f) d += 6.28318531f;
            if (std::abs(d) <= gapHalfRad) continue; // dentro del gap -> no se emite bala
        }

        m_emitter->emit(m_position, a, speed);
    }
}

void Boss::emitToPlayerPulse(float speed) {
    if (!m_emitter) return;
    // uso la snapshot tomada al inicio de la wave
    const sf::Vector2f target = m_wave.playerSnapshot;
    const sf::Vector2f d = target - m_position;
    const float ang = std::atan2(d.y, d.x);
    m_emitter->emit(m_position, ang, speed);
}

void Boss::selectClosestActiveOrb()
{
    // Checks para rapida salida
    m_currentOrb = nullptr;
    if (!m_orbs) return;

    // inicio la distancia al cuadrado con un número grande
    float bestD2 = 1e9f;
    
    for (IOrb* o : *m_orbs)
    {
        if (!o || !o->isActive()) continue;
        const sf::Vector2f p = o->getPosition();

        // Vector entre el boss y el orbe
        const float dx = (p.x - m_position.x);
        const float dy = (p.y - m_position.y);

        // Distancia al cuadrado (no uso sqrt porque dicen que baja el performance)
        const float d2 = dx*dx + dy*dy;

        // Me quedo con el orbe con menor distancia al cuadrado
        if (d2 < bestD2) { bestD2 = d2; m_currentOrb = o; }

        // Podria comparar con el absoluto de dx ( |dx| ) para su alineacion horizontal pero esta manera me parece mas comoda
    }
}

bool Boss::hasActiveOrbs() const
{
    if (!m_orbs) return false;
    for (IOrb* o : *m_orbs)
    {
        if (o && o->isActive()) return true;
    }
    
    return false;
}

void Boss::phaseChange()
{
    if      (m_phase == Phase::P1) setPhase(Phase::P2);
    else if (m_phase == Phase::P2) setPhase(Phase::P3);
}

void Boss::resetAttackCooldown()
{
    m_attackCooldown = frand(m_cfg.cdMin, m_cfg.cdMax);
}

void Boss::updateBodyVisual()
{
    if (m_hasSprite) m_sprite.setPosition(m_position);
    // se esconde el beam a menos que este absorbiendo (el draw decide)
}

void Boss::updateBeamVisual()
{
    if (!m_currentOrb) return;

    const sf::Vector2f a = m_position;
    const sf::Vector2f b = m_currentOrb->getPosition();
    const sf::Vector2f d = b - a;

    const float len = std::sqrt(d.x*d.x + d.y*d.y);
    const float ang = std::atan2(d.y, d.x);

    m_beam.setPosition(a);
    m_beam.setRotation(sf::radians(ang));
    
    m_beam.setSize({ len, m_beam.getSize().y });

    m_beamGlow.setPosition(a);
    m_beamGlow.setRotation(sf::radians(ang));
    m_beamGlow.setSize({ len, m_beamGlow.getSize().y });
}

void Boss::clearBeam()
{
    // se apagan ambos
    m_beam.setSize({ 0.f, m_cfg.beamBaseThickness });
    m_beamGlow.setSize({ 0.f, m_cfg.beamBaseThickness * 1.8f });
}

void Boss::updateBeamStyle(float dt)
{
    // Pulso seno, thickness = base + amp * sin(2pi f t)
    m_beamPulseT += dt;
    const float twopi = 6.28318531f;
    const float thick = m_cfg.beamBaseThickness
        + m_cfg.beamPulseAmp * std::sin(twopi * m_beamPulseT * m_cfg.beamPulseHz);

    // Mantiene longitud actual (X) y solo ajusta el grosor (Y)
    const float len = m_beam.getSize().x;

    m_beam.setSize({ len, std::max(1.f, thick) });
    m_beamGlow.setSize({ len, std::max(1.f, thick * 1.8f) });

    // Alinea glow con el beam (posicion y rotacion ya seteado en updateBeamVisual)
    m_beamGlow.setPosition(m_beam.getPosition());
    m_beamGlow.setRotation(m_beam.getRotation());
}

void Boss::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    //if (m_sprite) target.draw(*m_sprite, states);

    if (m_hasSprite)                 
        target.draw(m_sprite, states);


    // Solo hace el draw del beam cuando absorbe y el orbe sigue activo
    // No tengo un (m_fsm.current()), podria crearme algo asi, peero
    // Como parche hago esto: Hago el draw si hay tamaño > 0.5f (soy una rata xdd)
    if (m_beam.getSize().x > 0.5f) {
        target.draw(m_beamGlow, states); // primero el halo
        target.draw(m_beam, states);     // arriba el beam solido
    }
}

std::unique_ptr<Boss> Boss::CreateDefault(
    ResouceManager& rm,
    sf::Vector2f worldPos,
    float scale,
    const Params& p,
    IBulletEmitter* emitter,
    const std::vector<IOrb*>* orbs)
{
    // Spritesheet y rects por defecto (ajustá si tus coords reales difieren)
    VisualConfig cfg;
    cfg.sheetPath = "../../res/Assets/Sprites/Boss/BossSpriteSheet1.png";
    cfg.rects.p1 = {{0, 0}, {207, 300} };
    cfg.rects.p2 = {{207, 0}, {229, 300} };
    cfg.rects.p3 = {{438, 0}, {220, 300 } };

    auto boss = std::make_unique<Boss>(
        rm, cfg.sheetPath,
        std::optional<sf::IntRect>{ cfg.rects.p1 },
        worldPos, scale, p, emitter, orbs);

    boss->setSprite(rm, cfg);     // registra rects y deja fase 1
    boss->setPhase(Phase::P1);
    return boss;
}

std::unique_ptr<Boss> Boss::Create(
    ResouceManager& rm,
    sf::Vector2f worldPos,
    float scale,
    const Params& p,
    IBulletEmitter* emitter,
    const std::vector<IOrb*>* orbs,
    const VisualConfig& cfg)
{
    auto boss = std::make_unique<Boss>(
        rm, cfg.sheetPath,
        std::optional<sf::IntRect>{ cfg.rects.p1 },
        worldPos, scale, p, emitter, orbs);

    boss->setSprite(rm, cfg);
    boss->setPhase(Phase::P1);
    return boss;
}

void Boss::setSprite(ResouceManager& rm, const VisualConfig& cfg)
{
    // Cargar textura y setear sprite base
    m_vcfg = cfg;
    m_tex = rm.getTexture(cfg.sheetPath);
    m_sprite = sf::Sprite(*m_tex);
    m_tex->setSmooth(true);

    setPhaseRects(cfg.rects);
    
    // Guardar rects por fase y aplica el de la fase actual
    m_hasSprite = true;
    applyRect();
}





