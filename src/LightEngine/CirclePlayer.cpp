#include "CircleEntity.h"
#include "CirclePlayer.h"
#include "CircleFragment.h"
#include "SampleScene.h"
#include <cmath>

static sf::Vector2f NormalizeVec(sf::Vector2f v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    if (len < 0.0001f) return { 1.f, 0.f };
    return { v.x / len, v.y / len };
}

void CirclePlayer::TrySplit()
{
    if (mIsSplit) return;
    float r = GetRadius();
    if (r < kMinSplitRadius * 1.5f) return;

    float newR = r / std::sqrt(2.f);
    SetRadius(newR);

    // Direction = direction du mouvement courant (ou droite par défaut)
    sf::Vector2f launchDir = NormalizeVec(mDirection);
    if (launchDir.x == 0.f && launchDir.y == 0.f)
        launchDir = { 1.f, 0.f };

    auto* scene    = GetScene<SampleScene>();
    auto* fragment = scene->CreateAndRegister<CircleFragment>(newR, mShape.getFillColor());
    fragment->SetPosition(GetPosition().x, GetPosition().y);
    fragment->SetDirection(launchDir.x, launchDir.y, CircleFragment::kLaunchSpeed);
    fragment->SetTag(3);   // tag joueur pour que les ennemis le reconnaissent
    fragment->mOwner  = this;

    mFragment = fragment;
    mIsSplit  = true;
}

void CirclePlayer::OnUpdate()
{
    sf::Vector2f dir(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) dir.y--;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dir.y++;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) dir.x--;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dir.x++;

    if (dir.x != 0.f || dir.y != 0.f)
    {
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
        dir /= length;
        mDirection = dir;   // mémorise pour TrySplit
        SetDirection(dir.x, dir.y, 250.f);
    }
    else
    {
        SetSpeed(0.f);
    }

    // Détection front montant sur Espace (évite le split répété si on maintient)
    bool spaceNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
    if (spaceNow && !mSpaceWasPressed)
        TrySplit();
    mSpaceWasPressed = spaceNow;
}
