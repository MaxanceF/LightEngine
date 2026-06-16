#pragma once
#include "Entity.h"
#include "Wall.h"

class DummyEntity : public Entity
{
	
	Entity* pEntity = nullptr;
	sf::Vector2f mLastDirection = {0.0f, 0.0f};
	sf::Vector2f mCubeStartPos = {0.0f, 0.0f};
	float mInvincibilityTime = 0.0f;
	bool mIsInvincible = false;
	
public:
	DummyEntity();
	void OnCollision(Entity* other) override;
	void OnUpdate() override;
};

