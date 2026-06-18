#include "DummyEnemy.h"

#include "SampleScene.h"



void DummyEnemy::Inputs()
{
    constexpr float speed = 150.f;

    if (mLastDirection == sf::Vector2f{0.f, 0.f})
    {
        mLastDirection = {1.f, 0.f};
        mWantedDirection = mLastDirection;

        SetDirection(
            mLastDirection.x,
            mLastDirection.y,
            speed
        );

        GridUpdate(2);
        return;
    }

    int gridX = static_cast<int>(GetPosition().x / 10.f);
    int gridY = static_cast<int>(GetPosition().y / 10.f);

    auto IsFree = [&](int x, int y)
    {
        auto* scene = GetScene<SampleScene>();

        if (x < 0 || x >= scene->GRID_WIDTH ||
            y < 0 || y >= scene->GRID_HEIGHT)
        {
            return false;
        }

        return scene->GetCell(x, y) == 0;
    };

    auto IsOpposite = [](const sf::Vector2f& a,
                         const sf::Vector2f& b)
    {
        return a.x == -b.x &&
               a.y == -b.y;
    };

    auto CountFreeCells =
        [&](int x, int y, int dx, int dy)
    {
        int count = 0;

        while (IsFree(x, y))
        {
            count++;

            x += dx;
            y += dy;
        }

        return count;
    };

    if (CanTurnNow())
    {
        bool danger = false;

        constexpr int lookAhead = 5;

        for (int i = 1; i <= lookAhead; ++i)
        {
            int checkX =
                gridX +
                static_cast<int>(mLastDirection.x) * i;

            int checkY =
                gridY +
                static_cast<int>(mLastDirection.y) * i;

            if (!IsFree(checkX, checkY))
            {
                danger = true;
                break;
            }
        }

        bool randomDecision =
            (std::rand() % 100) < 5; // 5%

        if (danger || randomDecision)
        {
            struct Choice
            {
                sf::Vector2f dir;
                int score;
            };

            std::vector<Choice> choices;

            std::vector<sf::Vector2f> directions =
            {
                { 1.f,  0.f},
                {-1.f,  0.f},
                { 0.f,  1.f},
                { 0.f, -1.f}
            };

            for (auto& dir : directions)
            {
                if (IsOpposite(dir, mLastDirection))
                    continue;

                int nextX =
                    gridX + static_cast<int>(dir.x);

                int nextY =
                    gridY + static_cast<int>(dir.y);

                if (!IsFree(nextX, nextY))
                    continue;

                int score =
                    CountFreeCells(
                        nextX,
                        nextY,
                        static_cast<int>(dir.x),
                        static_cast<int>(dir.y));

                choices.push_back({dir, score});
            }

            if (!choices.empty())
            {
                std::sort(
                    choices.begin(),
                    choices.end(),
                    [](const Choice& a,
                       const Choice& b)
                    {
                        return a.score > b.score;
                    });

                if (choices.size() >= 2 &&
                    (std::rand() % 100) < 15)
                {
                    mWantedDirection =
                        choices[1].dir;
                }
                else
                {
                    mWantedDirection =
                        choices[0].dir;
                }
            }
        }

        if (mWantedDirection != mLastDirection)
        {
            mLastDirection = mWantedDirection;

            SetDirection(
                mLastDirection.x,
                mLastDirection.y,
                speed
            );

            pEntity = CreateEntity<Wall>(
                sf::Vector2f{20.f, 20.f},
                sf::Color::Red
            );

            mCubeStartPos = GetPosition(0.5f, 0.5f);
            constexpr float TILE_SIZE = 10.f;

            mCubeStartPos.x =
                std::round(mCubeStartPos.x / TILE_SIZE) * TILE_SIZE;

            mCubeStartPos.y =
                std::round(mCubeStartPos.y / TILE_SIZE) * TILE_SIZE;
        }
    }

    GridUpdate(2);
}

bool DummyEnemy::IsOpposite(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return a.x == -b.x && a.y == -b.y;
}




