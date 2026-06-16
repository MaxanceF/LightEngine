#pragma once
#include "Entity.h"

class CircleEntity : public Entity
{
protected:
	float _eatMultiplier = 0.1f;
	float _collisionThreshold = 1.1f; // 10% Plus gros minimum
	
public:
	void OnCollision(Entity* other) override;
	void OnInitialize();
};

