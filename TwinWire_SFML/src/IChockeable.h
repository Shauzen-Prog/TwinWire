#pragma once
#include <SFML/Graphics.hpp>

//Interfaz para objetos que pueden ser "apretados" por los filamentos
class IChockeable {
public:
	virtual ~IChockeable() = default;

	// Invocado cuando se aprieta entre las puntas A y B.
	virtual void onChoke(const sf::Vector2f& aTip, const sf::Vector2f& bTip) = 0;

	// AABB del objeto para test de colisión rápido
	virtual sf::FloatRect bounds() const = 0;

	// Render básico para el prototipo (Eliminar para producción!)
	virtual void draw(sf::RenderTarget& rt) const = 0;
};