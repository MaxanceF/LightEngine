#pragma once
#include "Entity.h"

class DummyEntity : public Entity
{
	
protected:	
	Entity* pEntity = nullptr;
	sf::Vector2f mLastDirection = {0.0f, 0.0f};
	float mInvincibilityTime = 0.0f;
	bool mIsInvincible = false;
	sf::Vector2i mWantedDirection{1, 0};
	sf::Vector2i mDirection{1, 0};
	bool CanTurnNow() const;

	struct GridPos
	{
		int x;
		int y;
	};

	GridPos mGridPos;


	float mMoveTimer = 0.f;
	float mMoveDelay = 0.05f; // 20 cases/sec
	
public:
	DummyEntity();
	void OnUpdate() override;
	void Crash();
	virtual void Inputs();
	void SetMGridPos(int x, int y);
};

