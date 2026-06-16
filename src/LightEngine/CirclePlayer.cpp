#include "CircleEntity.h"
#include "CirclePlayer.h"
#include "SampleScene.h"
#include <cmath>

static sf::Vector2f NormalizeVec(sf::Vector2f v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len < 0.0001f) return { 1.f, 0.f };
    return { v.x / len, v.y / len };
}

void CirclePlayer::OnUpdate()
{
    float dt = GetDeltaTime();

    sf::RenderWindow* window = GetScene()->GetRenderWindow();
    sf::Vector2i mousePixel  = sf::Mouse::getPosition(*window);
    sf::Vector2f mousePos    = window->mapPixelToCoords(mousePixel);

    sf::Vector2f myPos   = GetPosition();
    sf::Vector2f toMouse = mousePos - myPos;
    float dist = std::sqrt(toMouse.x * toMouse.x + toMouse.y * toMouse.y);
    
    if (mDashSpeed > 1.f)
    {
        mDashSpeed -= mDashSpeed * mDashFriction * dt;
        if (mDashSpeed < 1.f) mDashSpeed = 0.f;
    }

    float r         = std::max(GetRadius(), 10.f);
    float baseSpeed = std::clamp(250.f * std::pow(20.f / r, 0.4f), 50.f, 400.f);
    
    if (dist > GetRadius())
        mDirection = { toMouse.x / dist, toMouse.y / dist };
    
    float totalSpeed = (dist > GetRadius() ? baseSpeed : 0.f) + mDashSpeed;

    if (totalSpeed > 0.f)
        SetDirection(mDirection.x, mDirection.y, totalSpeed);
    else
        SetSpeed(0.f);

    bool spaceNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (spaceNow && !mSpaceWasPressed)
        TrySplit();
    mSpaceWasPressed = spaceNow;
}

void CirclePlayer::TrySplit()
{
    if (GetRadius() < 15.f) return;
    AddRadius(-GetRadius() * 0.3f);

    float spawnDist = GetRadius() * 2.f;
    auto* child = CreateEntity<CirclePlayer>(GetRadius(), sf::Color::White);
    child->mSpaceWasPressed = true;
    child->_isChild         = true;
    child->mDirection       = mDirection;
    child->mDashSpeed       = 520.f;
    child->mDashFriction    = 2.8f;
    
    child->SetPosition(
        GetPosition().x + mDirection.x * spawnDist,
        GetPosition().y + mDirection.y * spawnDist
    );
    child->SetTag(3);

    GetScene<SampleScene>()->AddEntity(child);
}

void CirclePlayer::OnCollision(Entity* other)
{
    if (GetRadius() < other->GetRadius() * _collisionThreshold) return;

    float multiplier = other->IsTag(this->mTag) ? 0.7f : _eatMultiplier;

    if (other->IsTag(3))
    {
        CirclePlayer* otherPlayer = static_cast<CirclePlayer*>(other);
        if (otherPlayer->mDashSpeed > 50.f || mDashSpeed > 50.f) return;
    }
    SetOrigin(0.5f, 0.5f);
    AddRadius(other->GetRadius() * multiplier);
    SetOrigin(0.0f, 0.0f);
    GetScene<SampleScene>()->DeleteEntity(other);
}