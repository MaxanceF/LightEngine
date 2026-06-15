#include "SampleScene.h"

#include "CircleEntity.h"
#include "CirclePlayer.h"
#include "CircleEnemy.h"

#include "Debug.h"

#include <algorithm>
#include <random>

int SampleScene::RandomNumber(int min, int max)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

void SampleScene::OnInitialize()
{
    // INIT FOOD
    for (int i = 0; i < 100; i++)
    {
        auto* circle = CreateEntity<CircleEntity>(RandomNumber(3, 6), sf::Color::Red);
        circle->SetPosition(RandomNumber(0, 1280), RandomNumber(0, 720));
        circle->SetTag(1);
        _entities.push_back(circle);
    }

    // INIT ENEMIES
    for (int i = 0; i < 5; i++)
    {
        auto* circle = CreateEntity<CircleEnemy>(RandomNumber(20, 25), sf::Color::Blue);
        circle->SetPosition(RandomNumber(0, 1280), RandomNumber(0, 720));
        circle->SetTag(2);
        _entities.push_back(circle);
    }

    // INIT PLAYER
    auto* circle = CreateEntity<CirclePlayer>(23, sf::Color::White);
    circle->SetPosition(RandomNumber(0, 1280), RandomNumber(0, 720));
    circle->SetTag(3);
    _entities.push_back(circle);
}

void SampleScene::OnEvent(const sf::Event& event)
{
}

void SampleScene::OnUpdate()
{
    float dt = GetDeltaTime();

    // Spawn food périodique
    mFoodSpawnTimer += dt;
    if (mFoodSpawnTimer >= mFoodSpawnCooldown)
    {
        mFoodSpawnTimer = 0.f;

        auto* circle = CreateEntity<CircleEntity>(RandomNumber(3, 6), sf::Color::Red);
        circle->SetPosition(RandomNumber(0, 1280), RandomNumber(0, 720));
        circle->SetTag(1);
        _entities.push_back(circle);
    }
    
    for (float& timer : mPendingEnemyRespawns)
        timer -= dt;
    
    auto it = std::remove_if(
        mPendingEnemyRespawns.begin(),
        mPendingEnemyRespawns.end(),
        [](float t) { return t <= 0.f; }
    );

    int count = static_cast<int>(std::distance(it, mPendingEnemyRespawns.end()));
    mPendingEnemyRespawns.erase(it, mPendingEnemyRespawns.end());
    

    for (int i = 0; i < count; i++)
        SpawnEntity();
}

void SampleScene::DeleteEntity(Entity* entity)
{
    if (entity->IsTag(3))  
    {
        GameManager::Get()->TriggerScreenMelt();
    }
    if (entity->IsTag(2))
        mPendingEnemyRespawns.push_back(3.f);
    
    auto it = std::find(_entities.begin(), _entities.end(), entity);
    if (it != _entities.end())
        _entities.erase(it);

    entity->Destroy();
}

void SampleScene::SpawnEntity()
{
    auto* circle = CreateEntity<CircleEnemy>(RandomNumber(20, 25), sf::Color::Blue);
    circle->SetPosition(RandomNumber(0, 1280), RandomNumber(0, 720));
    circle->SetTag(2);
    _entities.push_back(circle);
}
