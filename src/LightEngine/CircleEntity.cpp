#include "CircleEntity.h"
#include "CirclePlayer.h"
#include "SampleScene.h"
#include <iostream>

void CircleEntity::OnInitialize()
{
    SetRigidBody(true);
    
}

void CircleEntity::OnCollision(Entity* other)
{
    if (GetRadius() >= other->GetRadius() * _collisionThreshold)
    {
        SetOrigin(0.5f, 0.5f);
        AddRadius(other->GetRadius() * _eatMultiplier);
        SetOrigin(0.0f, 0.0f);
        auto* scene = GetScene<SampleScene>();

        scene->DeleteEntity(other);
        sf::Vector2i mousePos = sf::Mouse::getPosition(*GetScene()->GetRenderWindow());
    }
}