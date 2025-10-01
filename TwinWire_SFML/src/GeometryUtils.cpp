#include "GeometryUtils.h"
#include <SFML/Graphics.hpp>
#include <cmath>     // std::fabs
#include <algorithm> // std::min, std::max

// Resultado del recorte segmento-rect
struct ClipResult {
	bool hit{false};
	float tEnter{0.f};      // parámetro t de entrada [0..1]
	sf::Vector2f point{};   // punto de impacto
};

// Liang–Barsky: recorta el segmento a..b contra un rect dado por sus 4 bordes
inline ClipResult SegmentVsRectEdges(sf::Vector2f a, sf::Vector2f b,
									 float left, float top,
									 float right, float bottom)
{
	const sf::Vector2f d = { b.x - a.x, b.y - a.y };
	float t0 = 0.f, t1 = 1.f;

	auto clip = [&](float p, float q)->bool {
		if (std::fabs(p) < 1e-6f) return q >= 0.f;   // paralelo al borde
		const float t = q / p;
		if (p < 0.f) { if (t > t1) return false; if (t > t0) t0 = t; }
		else         { if (t < t0) return false; if (t < t1) t1 = t; }
		return true;
	};

	// Recortes contra cada borde usando los bordes explícitos
	if (!clip(-d.x, a.x - left))   return {false, 0.f, {}};
	if (!clip( d.x, right  - a.x)) return {false, 0.f, {}};
	if (!clip(-d.y, a.y - top))    return {false, 0.f, {}};
	if (!clip( d.y, bottom - a.y)) return {false, 0.f, {}};

	// t0 es el parámetro de entrada al rect
	return { true, t0, { a.x + d.x * t0, a.y + d.y * t0 } };
}

inline ClipResult SegmentVsAABB(sf::Vector2f a, sf::Vector2f b, const sf::FloatRect& r)
{
	const float left   = RECT_LEFT(r);
	const float top    = RECT_TOP(r);
	const float right  = RECT_RIGHT(r);
	const float bottom = RECT_BOTTOM(r);
	return SegmentVsRectEdges(a, b, left, top, right, bottom);
}

inline bool SegmentIntersectsAABB(const sf::Vector2f& p0, const sf::Vector2f& p1, const sf::FloatRect& r)
{
	return SegmentVsAABB(p0, p1, r).hit;
}
