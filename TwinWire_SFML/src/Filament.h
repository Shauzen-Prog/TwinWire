#pragma once
#include <SFML/Graphics.hpp>
#include "StateMachine.h"

enum class FilamentState { Idle, Aiming, Attached, Cooldown};

class Filament 
{
public:
	Filament(float thickness = 3.f); //constructor con grosor
	Filament(); // constructor por defecto

	// API pública que dispara transiciones
	void beginAim();
	void attachTo(const sf::Vector2f& origin, const sf::Vector2f& target);
	void release();
	bool isAttached() const { return m_state.get() == FilamentState::Attached; }

	void updateOrigin(const sf::Vector2f& origin);
	void update(float dt);
	void draw(sf::RenderTarget& target) const;

private:
	sf::RectangleShape m_beam;
	sf::Vector2f m_origin {};
	sf::Vector2f m_target {};
	float m_cooldown {0.f};

	// FSM
	StateMachine<FilamentState, Filament> m_state;

	// Helpers
	void refreshBeam();
};