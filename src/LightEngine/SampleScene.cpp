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
sf::Vector2f SampleScene::FindFreePosition(float radius)
{
    const int kMaxTries = 50;

    for (int attempt = 0; attempt < kMaxTries; attempt++)
    {
        float x = (float)RandomNumber((int)radius, 1280 - (int)radius);
        float y = (float)RandomNumber((int)radius, 720  - (int)radius);

        bool overlaps = false;
        for (CircleEntity* e : _entities)
        {
            sf::Vector2f pos = e->GetPosition();
            float dx = pos.x - x;
            float dy = pos.y - y;
            float minDist = e->GetRadius() + radius;
            if (dx*dx + dy*dy < minDist * minDist)
            {
                overlaps = true;
                break;
            }
        }

        if (!overlaps)
            return { x, y };
    }

    // Fallback si aucune position libre trouvée après kMaxTries
    return {
        (float)RandomNumber((int)radius, 1280 - (int)radius),
        (float)RandomNumber((int)radius, 720  - (int)radius)
    };
}

void SampleScene::SpawnEntity()
{
    float radius = (float)RandomNumber(20, 25);
    auto* circle = CreateEntity<CircleEnemy>(radius, sf::Color::Blue);
    sf::Vector2f pos = FindFreePosition(radius);
    circle->SetPosition(pos.x, pos.y);
    circle->SetTag(2);
    _entities.push_back(circle);
}

void SampleScene::AddEntity(CircleEntity* entity)
{
    // Repositionner si overlap au spawn (cas du split joueur)
    sf::Vector2f pos = entity->GetPosition();
    float r = entity->GetRadius();

    for (CircleEntity* e : _entities)
    {
        if (e == entity) continue;
        sf::Vector2f epos = e->GetPosition();
        float dx = epos.x - pos.x;
        float dy = epos.y - pos.y;
        float minDist = e->GetRadius() + r;
        if (dx*dx + dy*dy < minDist * minDist)
        {
            // Pousse l'entité hors du collider
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist < 0.001f) dist = 1.f;
            float push = (minDist - dist) + 1.f;
            pos.x -= (dx / dist) * push;
            pos.y -= (dy / dist) * push;
        }
    }

    entity->SetPosition(pos.x, pos.y);
    _entities.push_back(entity);
}

void SampleScene::OnInitialize()
{
    for (int i = 0; i < 100; i++)
    {
        float r = (float)RandomNumber(3, 6);
        auto* circle = CreateEntity<CircleEntity>(r, sf::Color::Red);
        sf::Vector2f pos = FindFreePosition(r);
        circle->SetPosition(pos.x, pos.y);
        circle->SetTag(1);
        _entities.push_back(circle);
    }

    for (int i = 0; i < 20; i++)
    {
        float r = (float)RandomNumber(20, 25);
        auto* circle = CreateEntity<CircleEnemy>(r, sf::Color::Blue);
        sf::Vector2f pos = FindFreePosition(r);
        circle->SetPosition(pos.x, pos.y);
        circle->SetTag(2);
        _entities.push_back(circle);
    }

    float pr = 23.f;
    auto* player = CreateEntity<CirclePlayer>(pr, sf::Color::White);
    sf::Vector2f pos = FindFreePosition(pr);
    player->SetPosition(pos.x, pos.y);
    player->SetTag(3);
    _entities.push_back(player);
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
        int playerCount = 0;
        for (int i = 0; i < static_cast<int>(_entities.size()); i++)
        {
            if (_entities[i]->IsTag(3) && _entities[i] != entity)
                playerCount++;
        }

        //if (playerCount == 0)
            //GameManager::Get()->TriggerScreenMelt();
    }
    if (entity->IsTag(2))
        mPendingEnemyRespawns.push_back(3.f);
    
    auto it = std::find(_entities.begin(), _entities.end(), entity);
    if (it != _entities.end())
        _entities.erase(it);

    entity->Destroy();
}