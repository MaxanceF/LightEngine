#include "DummyEnemy.h"

#include "SampleScene.h"
#include "Utils.h"


void DummyEnemy::OnUpdate()
{
    mMoveTimer += GetDeltaTime();

    if (mMoveTimer < mMoveDelay)
        return;

    mMoveTimer -= mMoveDelay;

    auto* scene = GetScene<SampleScene>();

    auto IsFree = [&](int x, int y)
    {
        if (x < 0 || x >= scene->GRID_WIDTH ||
            y < 0 || y >= scene->GRID_HEIGHT)
        {
            return false;
        }

        return scene->GetCell(x, y) == 0;
    };

    auto TurnLeft = [](const sf::Vector2i& dir)
    {
        return sf::Vector2i(-dir.y, dir.x);
    };

    auto TurnRight = [](const sf::Vector2i& dir)
    {
        return sf::Vector2i(dir.y, -dir.x);
    };

    if (mDirection == sf::Vector2i(0, 0))
    {
        mDirection = {1, 0};
    }

    int random = std::rand() % 100;

    // 85% tout droit
    // 7.5% gauche
    // 7.5% droite
    if (random >= 85)
    {
        bool turnLeftFirst = (std::rand() % 2) == 0;

        sf::Vector2i leftDir = TurnLeft(mDirection);
        sf::Vector2i rightDir = TurnRight(mDirection);

        sf::Vector2i wantedDir =
            turnLeftFirst ? leftDir : rightDir;

        int testX =
            mGridPos.x + wantedDir.x;

        int testY =
            mGridPos.y + wantedDir.y;

        if (IsFree(testX, testY))
        {
            mDirection = wantedDir;
        }
        else
        {
            wantedDir =
                turnLeftFirst ? rightDir : leftDir;

            testX =
                mGridPos.x + wantedDir.x;

            testY =
                mGridPos.y + wantedDir.y;

            if (IsFree(testX, testY))
            {
                mDirection = wantedDir;
            }
        }
    }

    //
    // Sécurité : obstacle devant
    //
    int frontX =
        mGridPos.x + mDirection.x;

    int frontY =
        mGridPos.y + mDirection.y;

    if (!IsFree(frontX, frontY))
    {
        sf::Vector2i leftDir =
            TurnLeft(mDirection);

        sf::Vector2i rightDir =
            TurnRight(mDirection);

        bool leftFree =
            IsFree(
                mGridPos.x + leftDir.x,
                mGridPos.y + leftDir.y);

        bool rightFree =
            IsFree(
                mGridPos.x + rightDir.x,
                mGridPos.y + rightDir.y);

        if (leftFree && rightFree)
        {
            mDirection =
                (std::rand() % 2 == 0)
                ? leftDir
                : rightDir;
        }
        else if (leftFree)
        {
            mDirection = leftDir;
        }
        else if (rightFree)
        {
            mDirection = rightDir;
        }
        else
        {
            Crash();
            return;
        }
    }

    int nextX =
        mGridPos.x + mDirection.x;

    int nextY =
        mGridPos.y + mDirection.y;

    if (!IsFree(nextX, nextY))
    {
        Crash();
        return;
    }

    scene->SetCell(
        mGridPos.x,
        mGridPos.y,
        -1);

    mGridPos.x = nextX;
    mGridPos.y = nextY;

    scene->SetCell(
        mGridPos.x,
        mGridPos.y,
        2);

    constexpr float TILE_SIZE = 10.f;

    SetPosition(
        mGridPos.x * TILE_SIZE,
        mGridPos.y * TILE_SIZE);
}

int DummyEnemy::CountReachableCells(int startX, int startY)
{
    
    
    auto* scene = GetScene<SampleScene>();
    
    
    auto IsFree = [&](int x, int y)
    {
        if (x < 0 || x >= scene->GRID_WIDTH ||
            y < 0 || y >= scene->GRID_HEIGHT)
        {
            return false;
        }

        return scene->GetCell(x, y) == 0;
    };
    
    std::queue<sf::Vector2i> open;

    std::vector<std::vector<bool>> visited(
        scene->GRID_WIDTH,
        std::vector<bool>(
            scene->GRID_HEIGHT,
            false));

    open.push({startX, startY});
    visited[startX][startY] = true;

    int count = 0;

    constexpr int dirs[4][2] =
    {
        { 1,  0},
        {-1,  0},
        { 0,  1},
        { 0, -1}
    };
    
    return count;
}

DummyEnemy::DummyEnemy()
{
    mWantedDirection={1, 0};
    mDirection={1, 0};
}

bool DummyEnemy::IsOpposite(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return a.x == -b.x && a.y == -b.y;
}

sf::Vector2i TurnLeft(const sf::Vector2i& dir)
{
    return {-dir.y, dir.x};
}

sf::Vector2i TurnRight(const sf::Vector2i& dir)
{
    return {dir.y, -dir.x};
}


