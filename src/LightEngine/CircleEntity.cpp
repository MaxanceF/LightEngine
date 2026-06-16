#include "CircleEntity.h"
#include "CirclePlayer.h"
#include "SampleScene.h"

void CircleEntity::OnInitialize()
{
    SetRigidBody(true);
    _visualRadius = GetRadius();
    _targetRadius = GetRadius();
}

void CircleEntity::OnUpdate()
{
    if (_visualRadius < 0.f) _visualRadius = GetRadius();
    if (_targetRadius < 0.f) _targetRadius = GetRadius();

    _visualRadius += (_targetRadius - _visualRadius) * _smoothSpeed * GetDeltaTime();

    mShape.setRadius(_visualRadius);
}

void CircleEntity::OnCollision(Entity* other)
{
    if (IsTag(3) || other->IsTag(3)) return;

    if (GetRadius() >= other->GetRadius() * _collisionThreshold)
    {
        float newR = std::sqrt(GetRadius() * GetRadius()
                             + other->GetRadius() * other->GetRadius() * _eatMultiplier);
        SetRadius(newR); 
        GetScene<SampleScene>()->DeleteEntity(other);
    }
}