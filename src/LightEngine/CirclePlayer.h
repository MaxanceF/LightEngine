#pragma once
#include "CircleEntity.h"
#include "CircleFragment.h"  // réutilise le même fragment

class CirclePlayer : public CircleEntity
{
public:
	bool            mIsSplit  = false;
	CircleFragment* mFragment = nullptr;

	static constexpr float kMinSplitRadius = 14.f;

protected:
	void OnUpdate() override;

private:
	void TrySplit();
	bool mSpaceWasPressed = false;  // pour détecter le front montant
};