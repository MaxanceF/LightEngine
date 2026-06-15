#include "CircleEntity.h"
#include "CirclePlayer.h"
#include "SampleScene.h"
#include <iostream>

void CircleEntity::OnInitialize()
{
    
}

void CircleEntity::OnCollision(Entity* other)
{
    if (GetRadius() > other->GetRadius())
    {
        AddRadius(other->GetRadius() * 0.1f);
        auto* scene = GetScene<SampleScene>();

        scene->DeleteEntity(other);
    }
}