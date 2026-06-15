#include "CircleFragment.h"
#include "CircleEnemy.h"
#include "CirclePlayer.h"
#include "SampleScene.h"
#include <cmath>

void CircleFragment::OnUpdate()
{
    float dt = GetDeltaTime();
    mMergeTimer += dt;

    // Décélération
    float spd = mSpeed * (1.f - 2.5f * dt);
    if (spd < 1.f) spd = 0.f;
    SetSpeed(spd);

    if (mMergeTimer < kMergeDelay || !mOwner) return;

    sf::Vector2f myPos    = GetPosition();
    sf::Vector2f ownerPos = mOwner->GetPosition();
    float dx   = myPos.x - ownerPos.x;
    float dy   = myPos.y - ownerPos.y;
    float dist = std::sqrt(dx * dx + dy * dy);

    float mergeRange = mOwner->GetRadius() * 3.f;

    if (dist > mergeRange)
    {
        float len = dist > 0.f ? dist : 1.f;
        SetDirection((ownerPos.x - myPos.x) / len,
                     (ownerPos.y - myPos.y) / len,
                     100.f);
    }
    else
    {
        // Conservation de surface
        float r1 = mOwner->GetRadius();
        float r2 = GetRadius();
        mOwner->SetRadius(std::sqrt(r1 * r1 + r2 * r2));

        // Notifie l'owner selon son type
        if (mOwner->IsTag(2))
        {
            auto* enemy = static_cast<CircleEnemy*>(mOwner);
            enemy->mFragment = nullptr;
            enemy->mIsSplit  = false;
        }
        else if (mOwner->IsTag(3))
        {
            auto* player = static_cast<CirclePlayer*>(mOwner);
            player->mFragment = nullptr;
            player->mIsSplit  = false;
        }

        Destroy();
    }
}

void CircleFragment::OnCollision(Entity* other)
{
    if (other->IsTag(1) && other->GetRadius() < GetRadius())
    {
        float r1 = GetRadius(), r2 = other->GetRadius();
        SetRadius(std::sqrt(r1 * r1 + r2 * r2));
        GetScene<SampleScene>()->DeleteEntity(static_cast<CircleEntity*>(other));
    }
}