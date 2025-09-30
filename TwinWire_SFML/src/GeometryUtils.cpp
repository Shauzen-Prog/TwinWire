#include "GeometryUtils.h"

static inline bool clipLB(float p, float q, float& tmin, float& tmax) {
	if (p == 0.f) return q >= 0.f;
	float t = q / p;
	if (p < 0.f) { if (t > tmax) return false; if (t > tmin) tmin = t; }
	else { if (t < tmin) return false; if (t < tmax) tmax = t; }
	return true;
}

bool segmentIntersectsAABB(const sf::Vector2f& p0, const sf::Vector2f& p1, const sf::FloatRect& r) {
	sf::Vector2f d = p1 - p0;
	float tmin = 0.f, tmax = 1.f;
	if (!clipLB(-d.x, p0.x - RECT_LEFT(r), tmin, tmax)) return false;
	if (!clipLB(d.x, RECT_RIGHT(r) - p0.x, tmin, tmax)) return false;
	if (!clipLB(-d.y, p0.y - RECT_TOP(r), tmin, tmax)) return false;
	if (!clipLB(d.y, RECT_BOTTOM(r) - p0.y, tmin, tmax)) return false;
	return tmax >= tmin;
}
