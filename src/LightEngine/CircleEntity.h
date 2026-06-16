#pragma once
#include "Entity.h"

class CircleEntity : public Entity
{
protected:
	float _eatMultiplier      = 0.1f;
	float _collisionThreshold = 1.1f;

	float _visualRadius = -1.f;
	float _targetRadius = -1.f;
	float _smoothSpeed  = 0.8f;

public:
	void OnCollision(Entity* other) override;
	void OnInitialize() override;
	void OnUpdate() override;

	// Override pour passer par le système smooth
	void SetRadius(float radius) override 
	{
		//Entity::SetOrigin(0.5f, 0.5f);
		Entity::SetRadius(radius);   // hitbox logique instantanée
		//Entity::SetOrigin(0.0f, 0.0f);
		_targetRadius = radius;      // visuel lisse vers cette valeur
	}

	void AddRadius(float delta) override 
	{
		float newR = GetRadius() + delta;
		SetRadius(newR);             // passe par notre override
	}
};