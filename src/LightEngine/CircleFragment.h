#pragma once
#include "CircleEntity.h"

class CircleFragment : public CircleEntity
{
public:
    CircleEntity*  mOwner      = nullptr;
    float          mMergeTimer = 0.f;
    bool           mIsSplit    = false;   // copie locale, évite d'accéder aux membres privés de l'owner

    static constexpr float kMergeDelay  = 10.f;
    static constexpr float kLaunchSpeed = 480.f;

protected:
    void OnUpdate() override;
    void OnCollision(Entity* other) override;
};