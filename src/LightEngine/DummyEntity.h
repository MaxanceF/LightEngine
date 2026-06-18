#pragma once
#include "Entity.h"

class DummyEntity : public Entity
{
	
protected:	
	Entity* pEntity = nullptr;
	sf::Vector2f mLastDirection = {0.0f, 0.0f};
	sf::Vector2f mCubeStartPos = {0.0f, 0.0f};
	float mInvincibilityTime = 0.0f;
	bool mIsInvincible = false;
	void GridUpdate(int);
	sf::Vector2f mWantedDirection = {0.f, 0.f};
	bool CanTurnNow() const;

	
public:
	DummyEntity();
	void OnCollision(Entity* other) override;
	void OnUpdate() override;
	void Crash();
	virtual void Inputs();
	
};

