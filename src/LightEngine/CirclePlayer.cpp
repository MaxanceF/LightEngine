#include "CircleEntity.h"
#include "CirclePlayer.h"

void CirclePlayer::OnUpdate()
{
    sf::Vector2f dir(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
        dir.y--;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        dir.y++;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
        dir.x--;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        dir.x++;

    if (dir.x != 0.f || dir.y != 0.f)
    {
        float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);

        dir /= length;

        SetDirection(dir.x, dir.y, 250.f);
    }
    else
    {
        SetSpeed(0.f);
    }
}