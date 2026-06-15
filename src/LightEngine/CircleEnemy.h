#pragma once
#include "CircleEntity.h"

class CircleEnemy : public CircleEntity
{
private:
	float mVisionRange = 250.f;
	sf::Vector2f mVelocity{1.f, 0.f}; // jamais zéro

public:
	void OnUpdate() override;
};