#pragma once
#include <chrono>
#include <SFML/System/Vector2.hpp>

namespace Utils
{
	bool Normalize(sf::Vector2f& vector);
	float GetDistance(int x1, int y1, int x2, int y2);
	float GetAngleDegree(const sf::Vector2f& v1, const sf::Vector2f& v2);
	void StarChrono();
	void StopChrono();
	extern std::chrono::steady_clock::time_point beginChrono;
}