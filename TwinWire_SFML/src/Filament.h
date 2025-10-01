#pragma once
#include <SFML/Graphics.hpp>
#include "StateMachine.h"

class IChockeable; // forward

enum class FilamentState { Idle, Extending, Attached, Retracting, Cooldown };

class Filament 
{
public:
	Filament(float thickness = 3.f); //constructor con grosor
	Filament(); // constructor por defecto
	~Filament(); // nunca se si es realmente necesario

	void fireStraight(const sf::Vector2f& origin, const sf::Vector2f& mouseWorld, bool canAttach = true);
	// El Player le pasa la mano cada frame (si Lock está activo, se ignora durante la anim)
	void updateOrigin(const sf::Vector2f& world);

	void update(float dt);
	void draw(sf::RenderTarget& target) const;
	
	// Config
	
	const sf::Vector2f& origin() const { return m_origin; }
	const sf::Vector2f& target() const { return m_target; }

	// API pública que dispara transiciones
	void beginAim();
	void setAimTarget(const sf::Vector2f& target);
	void attachTo(const sf::Vector2f& origin, const sf::Vector2f& clickedWorld);
	void release();

	// Tuning
	void setMaxLength(float px){ m_maxLength = std::max(0.f, px); }
	void setExtendSpeed(float pxs){ m_extendSpeed = std::max(1.f, pxs); }
	void setRetractSpeed(float pxs){ m_retractSpeed = std::max(1.f, pxs); }
	void setLockOriginOnExtend(bool v){ m_lockOriginOnExtend = v; } // default true
	
	// Raycast callback. Devuelve el IChockeable impactado o nullptr.
	// Debe también escribir el punto de impacto en outHit si hay hit.
	using RaycastFn = std::function<IChockeable*(const sf::Vector2f& a,
												const sf::Vector2f& b,
												sf::Vector2f& outHit)>;
	void setRaycast(RaycastFn fn) { m_raycast = std::move(fn); }

	
	bool canFire() const {
		// bloquear si está en cooldown, visible (extend/retract) o attached
		return (m_cooldown <= 0.f) && (m_beam.getSize().x <= 0.5f) && !m_attached;
	}

	void forceRetract();
	
	// Estado/consulta
	
	const sf::Vector2f& attachPoint() const { return m_attachPoint; }
	IChockeable* attachedObject() const { return m_attachedObj; }

	bool isAttached() const { return m_attached; }
	bool isExtending()  const { return m_extending; }
	bool isRetracting() const { return m_retracting; }
	bool isVisible()    const { return m_beam.getSize().x > 0.5f; } // util para debug
	void setColor(const sf::Color& c) { m_beam.setFillColor(c); }

private:
	// Visual
	sf::RectangleShape m_beam;
	
	// Geometría
	sf::Vector2f m_origin{};        // mano (dinámico)
	sf::Vector2f m_lockedOrigin{};  // mano congelada al disparar
	sf::Vector2f m_target{};        // punta visible
	sf::Vector2f m_dir{};           // dirección normalizada

	// --- longitud actual en píxeles (crece/disminuye) ---
	float m_lenPx{0.f};
	float m_lenStart{0.f};

	// Lerp / trayectoria
	float m_maxLength{260.f};
	float m_extendSpeed{900.f};
	float m_retractSpeed{1200.f};
	float m_t{0.f};
	bool  m_lockOriginOnExtend{true};

	// Trazo/lerp
	sf::Vector2f m_startOrigin{}, m_startTarget{}, m_finalTarget{};
	float m_extendT{0.f}, m_retractT{0.f};
	bool  m_extending{false}, m_retracting{false};

	// Attach
	bool          m_canAttach{true};
	bool          m_attached{false};
	IChockeable*  m_attachedObj{nullptr};
	sf::Vector2f  m_attachPoint{};

	float m_cooldownTime { 1.5f }; // ajusto el default 
	float m_cooldown     { 0.f };   // tiempo restante

	// Raycast
	RaycastFn m_raycast{};

	// FSM
	StateMachine<FilamentState, Filament> m_state;

	// Interno
	void refreshBeam();
	static inline float vlen(sf::Vector2f v){ return std::sqrt(v.x*v.x + v.y*v.y); }
};