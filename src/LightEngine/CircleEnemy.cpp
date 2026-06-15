#include "CircleEnemy.h"
#include "SampleScene.h"

#include <cmath>

static sf::Vector2f Normalize(sf::Vector2f v)
{
    float len = std::sqrt(v.x * v.x + v.y * v.y);

    if (len < 0.0001f)
        return {1.f, 0.f};

    return {v.x / len, v.y / len};
}

static float Distance(sf::Vector2f a, sf::Vector2f b)
{
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return std::sqrt(dx * dx + dy * dy);
}

static bool IsSafeFood(sf::Vector2f pos)
{
    float margin = 80.f;

    return !(pos.x < margin || pos.x > 1280 - margin ||
             pos.y < margin || pos.y > 720 - margin);
}

static sf::Vector2f RandomDir()
{
    sf::Vector2f d(
        (float)(rand() % 3 - 1),
        (float)(rand() % 3 - 1)
    );

    if (d.x == 0.f && d.y == 0.f)
        return {1.f, 0.f};

    return Normalize(d);
}

void CircleEnemy::OnUpdate()
{
    auto* scene = GetScene<SampleScene>();

    sf::Vector2f myPos = GetPosition();

    CircleEntity* danger = nullptr;
    CircleEntity* prey = nullptr;
    CircleEntity* food = nullptr;

    float dangerDist = mVisionRange;
    float preyDist = mVisionRange;
    float foodDist = mVisionRange;

    // =========================
    // SCAN WORLD
    // =========================
    for (CircleEntity* e : scene->GetEntities())
    {
        if (e == this)
            continue;

        float dist = Distance(myPos, e->GetPosition());

        if (dist > mVisionRange)
            continue;

        // 🍎 FOOD
        if (e->IsTag(1))
        {
            if (!IsSafeFood(e->GetPosition()))
                continue;

            if (dist < foodDist)
            {
                food = e;
                foodDist = dist;
            }
        }

        // ⚔️ ENEMY
        if (e->IsTag(2))
        {
            if (e->GetRadius() > GetRadius())
            {
                if (dist < dangerDist)
                {
                    danger = e;
                    dangerDist = dist;
                }
            }
            else
            {
                if (dist < preyDist)
                {
                    prey = e;
                    preyDist = dist;
                }
            }
        }
    }

    // =========================
    // WALL AVOIDANCE (smooth center pull)
    // =========================
    float width = 1280.f;
    float height = 720.f;
    float margin = 20.f;

    sf::Vector2f avoid(0.f, 0.f);

    if (myPos.x < margin)
        avoid.x = (margin - myPos.x) / margin;

    if (myPos.x > width - margin)
        avoid.x = -(myPos.x - (width - margin)) / margin;

    if (myPos.y < margin)
        avoid.y = (margin - myPos.y) / margin;

    if (myPos.y > height - margin)
        avoid.y = -(myPos.y - (height - margin)) / margin;

    avoid = Normalize(avoid);

    float avoidWeight = 0.5f;

    if (myPos.x < margin || myPos.x > width - margin ||
        myPos.y < margin || myPos.y > height - margin)
    {
        avoidWeight = 2.5f;
    }

    // =========================
    // TARGET SELECTION
    // =========================
    sf::Vector2f targetDir(0.f, 0.f);
    bool hasTarget = false;

    // 🚨 danger -> flee
    if (danger)
    {
        targetDir = Normalize(myPos - danger->GetPosition());
        hasTarget = true;
    }
    // 🍴 prey -> chase
    else if (prey)
    {
        targetDir = Normalize(prey->GetPosition() - myPos);
        hasTarget = true;
    }
    // 🍎 food -> seek
    else if (food)
    {
        targetDir = Normalize(food->GetPosition() - myPos);
        hasTarget = true;
    }
    // 🌍 wander
    else
    {
        targetDir = RandomDir();
        hasTarget = true;
    }

    // safety
    if (!hasTarget)
        targetDir = RandomDir();

    // =========================
    // COMBINE STEERING
    // =========================
    sf::Vector2f finalDir = targetDir + avoid * avoidWeight;
    finalDir = Normalize(finalDir);

    if (finalDir.x == 0.f && finalDir.y == 0.f)
        finalDir = RandomDir();

    // =========================
    // SMOOTH MOVEMENT
    // =========================
    mVelocity = mVelocity + (finalDir - mVelocity) * 0.08f;

    if (std::sqrt(mVelocity.x * mVelocity.x + mVelocity.y * mVelocity.y) < 0.02f)
        mVelocity = RandomDir();

    SetDirection(mVelocity.x, mVelocity.y, 180.f);
}