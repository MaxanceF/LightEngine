#pragma once
#include "CircleEntity.h"

class CirclePlayer : public CircleEntity
{
public:
	bool            mIsSplit  = false;

	static constexpr float kMinSplitRadius = 14.f;

	bool _isChild = false;

	float mDashSpeed    = 0.f;
	float mDashFriction = 3.5f;   // facteur de décélération


protected:
	void OnUpdate() override;
	void OnCollision(Entity* other) override;

private:
	void TrySplit();
	bool mSpaceWasPressed = false;  // pour détecter le front montant
};