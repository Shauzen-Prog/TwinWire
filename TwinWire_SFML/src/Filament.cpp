#include "Filament.h"
#include <cmath>

Filament::Filament(float thickness)
: m_state(*this)
{
    m_beam.setSize({0.f, thickness});
    m_beam.setOrigin({0.f, thickness * 0.5f});
    m_beam.setFillColor(sf::Color(200, 220, 255));

    using S = FilamentState;

    // IDLE
    m_state.registerState(S::Idle, {
        /*enter*/  [](Filament& f){ f.m_beam.setSize({0.f, f.m_beam.getSize().y}); },
        /*update*/ [](Filament& /*f*/, float /*dt*/){},
        /*exit*/   nullptr
    });

    // AIMING (solo muestra una “línea” hasta el mouse)
    m_state.registerState(S::Aiming, {
        /*enter*/  nullptr,
        /*update*/ [](Filament& f, float /*dt*/){ f.refreshBeam(); },
        /*exit*/   nullptr
    });

    // ATTACHED (fijo a un target)
    m_state.registerState(S::Attached, {
        /*enter*/  [](Filament& f){ f.refreshBeam(); },
        /*update*/ [](Filament& f, float /*dt*/){ f.refreshBeam(); },
        /*exit*/   nullptr
    });

    // COOLDOWN (no se puede usar, solo corre el timer)
    m_state.registerState(S::Cooldown, {
        /*enter*/  [](Filament& f){ f.m_cooldown = 0.35f; }, //  350 ms
        /*update*/ [](Filament& f, float dt){
            f.m_cooldown -= dt;
            if (f.m_cooldown <= 0.f) f.m_state.change(S::Idle);
        },
        /*exit*/   nullptr
    });

    m_state.setInitial(S::Idle);
}

void Filament::beginAim() {
    if (m_state.get() == FilamentState::Idle)
        m_state.change(FilamentState::Aiming);
}

void Filament::attachTo(const sf::Vector2f& origin, const sf::Vector2f& target) {
    m_origin = origin;
    m_target = target;
    m_state.change(FilamentState::Attached);
}

void Filament::release() {
    if (m_state.get() == FilamentState::Attached || m_state.get() == FilamentState::Aiming) {
        m_state.change(FilamentState::Cooldown);
    }
}

void Filament::updateOrigin(const sf::Vector2f& origin) {
    m_origin = origin;
}

void Filament::update(float dt) {
    m_state.update(dt);
}

void Filament::draw(sf::RenderTarget& target) const {
    if (m_state.get() == FilamentState::Attached || m_state.get() == FilamentState::Aiming)
        target.draw(m_beam);
}

void Filament::refreshBeam() {
    const sf::Vector2f d = m_target - m_origin;
    const float length = std::sqrt(d.x * d.x + d.y * d.y);
    const float angleRad = std::atan2(d.y, d.x);

    m_beam.setPosition(m_origin);
    m_beam.setRotation(sf::radians(angleRad));
    m_beam.setSize({length, m_beam.getSize().y});
}